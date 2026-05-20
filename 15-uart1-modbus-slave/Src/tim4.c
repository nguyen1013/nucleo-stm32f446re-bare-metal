/*
 * tim4.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "tim4.h"

#define T35_TICKS 20   // 2ms

//volatile uint32_t msTicks = 0;

void Tim4_init(void) {
	// Enable clock TIM4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	// PSC: 90MHz / (8999+1) = 10kHz
	TIM4->PSC = 8999;

	// ARR: 10kHz / (9+1) = 1kHz => 1ms tick
	TIM4->ARR = 9;

	// Apply PSC+ARR
	TIM4->EGR = TIM_EGR_UG;

	// Clear flag
	TIM4->SR &= ~TIM_SR_UIF;

	// Enable interrupt
	TIM4->DIER |= TIM_DIER_UIE;

	// NVIC
	NVIC_SetPriority(TIM4_IRQn, 5);
	NVIC_EnableIRQ(TIM4_IRQn);

	// Start
	TIM4->CR1 |= TIM_CR1_CEN;
}

void TIM4_init_T35(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	TIM4->PSC = 8999;             // 0.1ms per tick
	TIM4->ARR = T35_TICKS - 1;    // 2ms

	TIM4->EGR = TIM_EGR_UG;

	TIM4->SR &= ~TIM_SR_UIF;
	TIM4->DIER |= TIM_DIER_UIE;

	NVIC_SetPriority(TIM4_IRQn, 4);
	NVIC_EnableIRQ(TIM4_IRQn);
}

