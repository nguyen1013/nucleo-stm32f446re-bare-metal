/*
 * tim5.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "tim5.h"

void Tim5_init(void) {
	// Enable TIM5 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	(void) RCC->APB1ENR; // ensure enable clock

	// Reset registers
	TIM5->CR1 = 0;
	TIM5->CR2 = 0;
	TIM5->SMCR = 0;
	TIM5->DIER = 0;

	// Reset counter và flag
	TIM5->CNT = 0;
	TIM5->SR = 0;

	// Timer clock
	// SYSCLK=180MHz, APB1=45MHz → TIMx = 90MHz
	uint32_t timer_clk = 90000000UL;

	// Prescaler for 1 MHz (1 tick = 1 µs)
	TIM5->PSC = (timer_clk / 1000000UL) - 1;   // eg. 90 - 1 = 89

	// Auto reload max (32-bit)
	TIM5->ARR = 0xFFFFFFFF;

	// Force update to load PSC
	TIM5->EGR = TIM_EGR_UG;

	// Enable counter
	TIM5->CR1 |= TIM_CR1_CEN;
}

void delay_us_tim5(uint32_t us) {
	if (us == 0)
		return;

	uint32_t start = TIM5->CNT;

	while ((uint32_t) (TIM5->CNT - start) < us) {
		// busy wait
	}
}
