/*
 * tim2.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "tim2.h"

void Tim2_init(void) {
	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	(void) RCC->APB1ENR; // dummy read to ensure clock is enabled

	// Reset basic registers
	TIM2->CR1 = 0;
	TIM2->CR2 = 0;
	TIM2->SMCR = 0;
	TIM2->DIER = 0;

	TIM2->CNT = 0;
	TIM2->SR = 0; // clear flags

	// NOTE: timer_clk should be computed from RCC settings in real projects
	uint32_t timer_clk = 90000000UL;

	// 1 tick = 1us
	TIM2->PSC = (timer_clk / 1000000UL) - 1;

	// Max ARR
	TIM2->ARR = 0xFFFFFFFF;

	// Force update event to load PSC
	TIM2->EGR = TIM_EGR_UG;

	// Enable counter
	TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_us_tim2(uint32_t us) {
	if (us == 0)
		return;

	uint32_t start = TIM2->CNT;
	while ((uint32_t) (TIM2->CNT - start) < us) {
		// busy wait
	}
}

