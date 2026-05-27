/*
 * encoder_PEC11R.h
 *
 *  Created on: 27.5.2026
 *      Author: nn
 */

#ifndef ENCODER_PEC11R_H
#define ENCODER_PEC11R_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

/*
 * PEC11R driver (bare-metal STM32F446)
 *
 * Encoder:
 *  - PA8 = TIM1_CH1 (TI1)
 *  - PA9 = TIM1_CH2 (TI2)
 *  - GPIO AF = AF1 for TIM1 on PA8/PA9
 *  - TIM1 Encoder Mode 1 (TI1): x2, count on TI1 edges, direction depends on TI2 level
 *
 * Button:
 *  - PB5 = EXTI line 5 (IRQ: EXTI9_5)
 *  - Debounce by TIM7 1ms tick: external volatile g_tim7_ms provided by tim7.h
 */

/* Change 1 if CW/CCW are reversed */
#define ENC_INVERT_DIR 0

/* Scale raw ticks -> step (Mode1 x2 => 2 tick/cycle) */
#define ENC_STEP_DIV 2

/* Use filter input capture to reduce bounce encoder */
#define ENC_USE_INPUT_FILTER 1

/* 0..15 filter value (IC1F/IC2F) */
#define ENC_INPUT_FILTER 4

/* ----- Button support ----- */
#define ENC_USE_BUTTON 1

/* Button pin PB5 */
#define ENC_BTN_GPIO GPIOB
#define ENC_BTN_PIN  5U

/* 0 = active-low (pressed reads 0), 1 = active-high */
#define ENC_BTN_ACTIVE_LEVEL 0

/* Debounce time (ms) */
#define ENC_BTN_DEBOUNCE_MS 30U

/* TIM7 ms tick from existing tim7 */
extern volatile uint32_t g_tim7_ms;

/* =========================
 * Types
 * ========================= */

typedef enum {
	ENC_DIR_NONE = 0, ENC_DIR_CW, ENC_DIR_CCW
} enc_dir_t;

typedef enum {
	BTN_RELEASED = 0, BTN_PRESSED, BTN_CLICKED /* one-shot event */
} btn_state_t;

typedef struct {
	int32_t count; /* scaled by ENC_STEP_DIV */
	enc_dir_t dir; /* NONE/CW/CCW from last ENC_Update() */
	btn_state_t btn; /* released/pressed/clicked */
} enc_state_t;

/* =========================
 * API
 * ========================= */

void ENC_Init(void);
void ENC_Update(void);
enc_state_t ENC_GetState(void);

/* Call this inside EXTI9_5_IRQHandler() */
void ENC_BTN_EXTI_IRQHandler(void);

#endif /* ENCODER_PEC11R_H */
