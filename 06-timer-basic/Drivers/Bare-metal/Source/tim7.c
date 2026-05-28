/*
 * tim7.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "tim7.h"

volatile uint32_t g_tim7_ms = 0;

void Tim7_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	TIM7->CR1 &= ~TIM_CR1_CEN;

	/* 90MHz / (8999+1) = 10kHz */
	TIM7->PSC = 8999;

	/* 10kHz / (9+1) = 1kHz => 1ms */
	TIM7->ARR = 9;

	TIM7->CNT = 0;
	TIM7->EGR = TIM_EGR_UG;

	TIM7->SR &= ~TIM_SR_UIF;

	/* enable interrupt */
	TIM7->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM7_IRQn);

	TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM7_IRQHandler(void) {
	if (TIM7->SR & TIM_SR_UIF) {
		TIM7->SR &= ~TIM_SR_UIF;

		g_tim7_ms++;
	}
}
