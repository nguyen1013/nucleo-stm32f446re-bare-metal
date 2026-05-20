/*
 * tim4.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "tim4.h"

#define TIM4CLK_HZ   45000000UL // APB1 45Mhz

// Select T35_US:
//  - baud > 19200  -> 1750us = 2000us (fixed)
//  - baud <=19200  -> calculated by baud (eg.9600/8N2 -> ~4010us = 5000us)

#define T35_US       2000    // for baud 115200

void TIM4_init_T35(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	TIM4->CR1 = 0;
	TIM4->PSC = (TIM4CLK_HZ / 1000000UL) - 1;  // 45MHz -> 1MHz => PSC=44
	TIM4->ARR = T35_US - 1;                    // 1750us -> ARR=1749

	TIM4->EGR = TIM_EGR_UG;
	TIM4->SR = 0;
	TIM4->DIER |= TIM_DIER_UIE;

	TIM4->CR1 |= TIM_CR1_OPM;                  // one-shot

	NVIC_SetPriority(TIM4_IRQn, 6);
	NVIC_EnableIRQ(TIM4_IRQn);

	TIM4->CR1 &= ~TIM_CR1_CEN;                 // no start at init
}

