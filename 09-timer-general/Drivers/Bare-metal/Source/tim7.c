/*
 * tim7.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "tim7.h"

void Tim7_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	TIM7->CR1 &= ~TIM_CR1_CEN;   // stop timer before config

	TIM7->PSC = 8999;           // 90MHz / (8999+1) = 10kHz
	TIM7->ARR = 9999;           // 10000 ticks = 1s

	TIM7->CNT = 0;

	TIM7->EGR = TIM_EGR_UG;     // force update

	TIM7->SR &= ~TIM_SR_UIF;   // clear UIF

	TIM7->CR1 |= TIM_CR1_CEN;   // start timer
}

void delay_s(uint32_t s) {
	while (s--) {
		TIM7->SR &= ~TIM_SR_UIF;              // clear flag
		while (!(TIM7->SR & TIM_SR_UIF)) {
			;
		}  // wait overflow (1s)
	}
}
