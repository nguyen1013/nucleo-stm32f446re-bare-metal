/*
 * timer.c
 *
 *  Created on: 15.5.2026
 *      Author: nn
 */

#include "tim6.h"

void Tim6_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM6->CR1 &= ~TIM_CR1_CEN;   // stop timer

	TIM6->PSC = 8999;           // 90 MHz / 9000 = 10 kHz
	TIM6->ARR = 9;              // 1 ms

	TIM6->CNT = 0;

	TIM6->EGR = TIM_EGR_UG;     // apply PSC

	TIM6->SR &= ~TIM_SR_UIF;    // clear flag

	TIM6->CR1 |= TIM_CR1_CEN;   // start timer
}

void delay_ms_tim6(uint32_t ms) {
	while (ms--) {
		TIM6->SR &= ~TIM_SR_UIF;   // clear flag
		while (!(TIM6->SR & TIM_SR_UIF))
			;  // wait overflow
	}
}
