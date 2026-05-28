/*
 * encoder_PEC11R.c
 *
 *  Created on: 27.5.2026
 *      Author: nn
 */

#include "encoder_PEC11R.h"

/* =========================
 * Internal state
 * ========================= */

static volatile int32_t g_count_raw = 0; // raw counting, not yet scaled by ENC_STEP_DIV
static uint16_t g_last_cnt = 0; // last counter (to calculte delta)
static volatile enc_state_t g_state = { 0, ENC_DIR_NONE, BTN_RELEASED }; // processed data for application

#if ENC_USE_BUTTON
/* Debounce state machine using g_tim7_ms */
static volatile uint8_t g_btn_pending = 0; /* flag for verify press button until end of debounce checking */
static uint32_t g_btn_deadline_ms = 0; /* time when debounce ends */
static uint8_t g_btn_stable_pressed = 0; /* stable level 0/1 */
static uint8_t g_btn_click_event = 0; /* one-shot click */
#endif

/* =========================
 * Helpers
 * ========================= */

static inline uint8_t read_btn_level(void) {
#if ENC_USE_BUTTON
	return (uint8_t) ((ENC_BTN_GPIO->IDR >> ENC_BTN_PIN) & 1U);
#else
    return 0;
#endif
}

static inline uint8_t is_btn_pressed(uint8_t level) {
#if ENC_BTN_ACTIVE_LEVEL
    return (level != 0U);
#else
	return (level == 0U);
#endif
}

/* =========================
 * TIM1 Encoder Mode 1 init
 * ========================= */

static void tim1_encoder_mode1_init(void) {
	/* clocks */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	/* GPIOA PA8/PA9 AF1 + pull-up */
	GPIOA->MODER &= ~((3U << (8U * 2U)) | (3U << (9U * 2U)));
	GPIOA->MODER |= ((2U << (8U * 2U)) | (2U << (9U * 2U))); /* AF */

	GPIOA->PUPDR &= ~((3U << (8U * 2U)) | (3U << (9U * 2U)));
	GPIOA->PUPDR |= ((1U << (8U * 2U)) | (1U << (9U * 2U))); /* PU */

	/* AF1 for TIM1 on PA8/PA9 */
	GPIOA->AFR[1] &= ~((0xFU << ((8U - 8U) * 4U)) | (0xFU << ((9U - 8U) * 4U)));
	GPIOA->AFR[1] |= ((0x1U << ((8U - 8U) * 4U)) | (0x1U << ((9U - 8U) * 4U)));

	/* TIM1 setup */
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM1->CNT = 0;
	TIM1->ARR = 0xFFFFU;

	/* CCMR1: CC1S=01(TI1), CC2S=01(TI2) set input capture mode */
	TIM1->CCMR1 &= ~((3U << 0) | (3U << 8));
	TIM1->CCMR1 |= ((1U << 0) | (1U << 8));

#if ENC_USE_INPUT_FILTER
	/* IC1F/IC2F filter (0..15) */
	TIM1->CCMR1 &= ~((0xFU << 4) | (0xFU << 12));
	TIM1->CCMR1 |= (((uint32_t) (ENC_INPUT_FILTER & 0xFU)) << 4)
			| (((uint32_t) (ENC_INPUT_FILTER & 0xFU)) << 12);
#endif

	/* CCER: enable both channels, rising polarity */
	TIM1->CCER &= ~((1U << 1) | (1U << 5)); /* CC1P=0, CC2P=0 */
	TIM1->CCER |= ((1U << 0) | (1U << 4)); /* CC1E=1, CC2E=1 */

	/* SMCR.SMS = 001 => Encoder mode 1 (TI1), x2 */
	TIM1->SMCR &= ~(7U << 0);
	TIM1->SMCR |= (1U << 0);

	TIM1->CR1 |= TIM_CR1_CEN;

}

/* =========================
 * Button init (PB5 EXTI)
 * ========================= */

#if ENC_USE_BUTTON
static void button_init_exti_pb5(void) {
	/* GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* PB5 input */
	GPIOB->MODER &= ~(3U << (ENC_BTN_PIN * 2U));

#if ENC_BTN_ACTIVE_LEVEL
	/* active-high => pull-down */
	GPIOB->PUPDR &= ~(3U << (ENC_BTN_PIN * 2U));
	GPIOB->PUPDR |= (2U << (ENC_BTN_PIN * 2U));
#else
	/* active-low => pull-up */
	GPIOB->PUPDR &= ~(3U << (ENC_BTN_PIN * 2U));
	GPIOB->PUPDR |= (1U << (ENC_BTN_PIN * 2U));
#endif

	/* SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Map PB5 to EXTI5 */
	/* EXTI5 at EXTICR[1] (lines 4..7), shift = (5%4)*4 = 4 */
	SYSCFG->EXTICR[1] &= ~(0xFU << 4);
	SYSCFG->EXTICR[1] |= (0x1U << 4); /* Port B = 0b0001 */

	/* EXTI config: rising + falling */
	EXTI->IMR |= (1U << ENC_BTN_PIN);
	EXTI->RTSR |= (1U << ENC_BTN_PIN);
	EXTI->FTSR |= (1U << ENC_BTN_PIN);

	/* NVIC: PB5 => EXTI9_5 */
	NVIC_EnableIRQ(EXTI9_5_IRQn);

	/* init stable state */
#if ENC_BTN_ACTIVE_LEVEL
	g_btn_stable_pressed = 1;
#else
	g_btn_stable_pressed = 0;
#endif
	g_btn_click_event = 0;
	g_btn_pending = 0;
	g_btn_deadline_ms = 0;
}
#endif

/* =========================
 * Public API
 * ========================= */

void ENC_Init(void) {
	tim1_encoder_mode1_init();

#if ENC_USE_BUTTON
	button_init_exti_pb5();
#endif

	__disable_irq();
	g_count_raw = 0;
	g_last_cnt = (uint16_t) TIM1->CNT;

	g_state.count = 0;
	g_state.dir = ENC_DIR_NONE;
#if ENC_USE_BUTTON
	g_state.btn = g_btn_stable_pressed ? BTN_PRESSED : BTN_RELEASED;
#else
    g_state.btn   = BTN_RELEASED;
#endif
	__enable_irq();
}

void ENC_Update(void) {
	/* --- Encoder: delta with wrap-safe int16_t --- */
	uint16_t now = (uint16_t) TIM1->CNT;
	int16_t d = (int16_t) (now - g_last_cnt);
	g_last_cnt = now;

	enc_dir_t dir = ENC_DIR_NONE;
	if (d > 0)
		dir = ENC_DIR_CW;
	else if (d < 0)
		dir = ENC_DIR_CCW;

#if ENC_INVERT_DIR
    if (dir == ENC_DIR_CW) dir = ENC_DIR_CCW;
    else if (dir == ENC_DIR_CCW) dir = ENC_DIR_CW;
    d = (int16_t)(-d);
#endif

	g_count_raw += (int32_t) d;

	int32_t scaled = g_count_raw;
#if (ENC_STEP_DIV > 1)
	scaled /= (int32_t) ENC_STEP_DIV;
#endif

	/* --- Button: debounce by time (g_tim7_ms) --- */
#if ENC_USE_BUTTON
	if (g_btn_pending) {
		/* signed compare to handle wrap */
		if ((int32_t) (g_tim7_ms - g_btn_deadline_ms) >= 0) {
			g_btn_pending = 0;

			uint8_t pressed = is_btn_pressed(read_btn_level());

			/* Click event: stable pressed -> stable released */
			if ((g_btn_stable_pressed == 1U) && (pressed == 0U)) {
				g_btn_click_event = 1U;
			}
			g_btn_stable_pressed = pressed;

		}
	}
#endif

	/* --- Publish state --- */
	__disable_irq();
	g_state.count = scaled;
	g_state.dir = dir;

#if ENC_USE_BUTTON
	if (g_btn_click_event) {
		g_state.btn = BTN_CLICKED;
		g_btn_click_event = 0; /* consume */
	} else {
		g_state.btn = g_btn_stable_pressed ? BTN_PRESSED : BTN_RELEASED;
	}
#else
    g_state.btn = BTN_RELEASED;
#endif
	__enable_irq();
}

enc_state_t ENC_GetState(void) {
	enc_state_t s;
	__disable_irq();
	s = g_state;
	__enable_irq();
	return s;
}

/* =========================
 * Button EXTI hook (PB5)
 * Call inside EXTI9_5_IRQHandler()
 * ========================= */

void ENC_BTN_EXTI_IRQHandler(void) {
#if ENC_USE_BUTTON
	uint32_t line = (1U << ENC_BTN_PIN);

	if (EXTI->PR & line) {
		EXTI->PR = line; /* clear pending */

		/* Start debounce window */
		g_btn_pending = 1U;
		g_btn_deadline_ms = g_tim7_ms + (uint32_t) ENC_BTN_DEBOUNCE_MS;
	}
#endif
}
