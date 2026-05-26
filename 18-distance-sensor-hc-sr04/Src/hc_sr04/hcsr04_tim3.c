/*
 * hcsr04_tim3.c
 *
 *  Created on: 26.5.2026
 *      Author: nn
 * TIM3_CH2 PB5 as Input Capture
 * PA8 as trigger
 */

#include "hcsr04_tim3.h"

#define NO_DETECTED_OBJECT_ERR 0

extern void delay_us_tim5(uint32_t us);

static volatile uint32_t s_ovf = 0; // overflow counter
static volatile uint32_t s_pulse_ticks = 0; // time of ECHO (us)
static volatile uint8_t s_done = 0; // flag measuring done
static volatile uint8_t s_wait_fall = 0; // waiting for edge trigger (0=rising, 1=falling)

// PA8 as trigger
static inline void trig_high(void) {
	GPIOA->BSRR = (1U << 8);
}
static inline void trig_low(void) {
	GPIOA->BSRR = (1U << (8 + 16));
}

static void HCSR04_Trigger10us(void) {
	trig_low();
	delay_us_tim5(2);
	trig_high();
	delay_us_tim5(15);     // TRIG >= 10us
	trig_low();
}

void HCSR04_TIM3_Init(void) {
	/* === CLOCK ENABLE === */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* === TRIG: PA8 output === */
	GPIOA->MODER &= ~(3U << (8 * 2));
	GPIOA->MODER |= (1U << (8 * 2));      // output
	GPIOA->OTYPER &= ~(1U << 8);           // push-pull
	GPIOA->OSPEEDR |= (3U << (8 * 2));     // high speed (optional)
	trig_low();

	/* === ECHO: PB5 as AF TIM3_CH2 ===
	 */
	GPIOB->MODER &= ~(3U << (5 * 2));
	GPIOB->MODER |= (2U << (5 * 2));      // AF mode
	GPIOB->PUPDR &= ~(3U << (5 * 2));      // no pull
	GPIOB->OSPEEDR |= (3U << (5 * 2));     // high speed (optional)

	// AFRL for PB5: bits [23:20]
	// TIM3 pins are AF2 on STM32F4
	GPIOB->AFR[0] &= ~(0xFU << (5 * 4));
	GPIOB->AFR[0] |= (2U << (5 * 4));   // AF2

	/* === TIM3 base: 1MHz counter (1 tick = 1us) ===
	 TIMx(APB1)=90MHz => PSC=89 => 1MHz
	 */
	TIM3->CR1 = 0;
	TIM3->PSC = 89;
	TIM3->ARR = 0xFFFF;
	TIM3->CNT = 0;

	/* === TIM3 CH2 Input Capture ===
	   CC2S=01 => IC2 mapped on TI2 (PB5) */
	TIM3->CCMR1 &= ~(3U << 8);
	TIM3->CCMR1 |=  (1U << 8);

	// IC2 filter = 0011 (debounce / reject short glitches)
	TIM3->CCMR1 &= ~(0xFU << 12);        // clear IC2F
	TIM3->CCMR1 |=  (0x3U << 12);        // set IC2F = 0011

	// IC2 prescaler = 0
	TIM3->CCMR1 &= ~(3U << 10);          // IC2PSC

	/* Polarity start rising edge (CC2P=0, CC2NP=0), enable capture */
	TIM3->CCER &= ~((1U << 5) | (1U << 7)); // CC2P, CC2NP = 0 => rising
	TIM3->CCER |= (1U << 4);               // CC2E=1

	/* Enable interrupts: CC2IE + UIE (overflow) */
	TIM3->DIER |= (1U << 2) | (1U << 0);    // CC2IE, UIE
	TIM3->SR = 0;

	NVIC_EnableIRQ(TIM3_IRQn);

	TIM3->CR1 |= (1U << 0);                 // CEN
}

uint32_t HCSR04_TIM3_ReadPulse_us(uint32_t timeout_us) {
	s_ovf = 0;
	s_pulse_ticks = 0;
	s_done = 0;
	s_wait_fall = 0;

	// Reset polarity to rising
	TIM3->CCER &= ~((1U << 5) | (1U << 7)); // rising
	TIM3->SR = 0;
	TIM3->CNT = 0;

	HCSR04_Trigger10us();

	while (!s_done && timeout_us--) {
		delay_us_tim5(1);
	}

	return s_done ? s_pulse_ticks : NO_DETECTED_OBJECT_ERR;
}

uint32_t HCSR04_TIM3_ReadDistance_cm(uint32_t timeout_us) {
	uint32_t pulse_us = HCSR04_TIM3_ReadPulse_us(timeout_us);

	if (pulse_us == NO_DETECTED_OBJECT_ERR) {
		return NO_DETECTED_OBJECT_ERR;
	} else {
		// distance(cm) ≈ pulse_us / 58
		return (pulse_us + 29) / 58;
	}

}

void TIM3_IRQHandler(void) {
	uint32_t sr = TIM3->SR;

// Overflow event (UIF)
	if (sr & (1U << 0)) {
		TIM3->SR &= ~(1U << 0);
		s_ovf++;
	}

// Capture event CH2 (CC2IF)
	if (sr & (1U << 2)) {
		TIM3->SR &= ~(1U << 2);

		if (s_wait_fall == 0) {
			// Rising edge: reset counter & overflow
			TIM3->CNT = 0;
			s_ovf = 0;

			// Switch to falling edge:
			// CC2P=1, CC2NP=0
			TIM3->CCER &= ~(1U << 7);
			TIM3->CCER |= (1U << 5);

			s_wait_fall = 1;
		} else {
			// Falling edge: capture width
			uint32_t ccr = TIM3->CCR2;
			s_pulse_ticks = (s_ovf * (TIM3->ARR + 1U)) + ccr;

			// Prepare for next measure: back to rising
			TIM3->CCER &= ~((1U << 5) | (1U << 7));
			s_wait_fall = 0;
			s_done = 1;
		}
	}
}

