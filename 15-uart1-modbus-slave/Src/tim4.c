/*
 * tim4.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "tim4.h"

#define TIM4CLK_HZ   45000000UL

// Chọn 1 trong 2:
//  - baud > 19200  -> 1750us (fixed)
//  - baud <=19200  -> tính theo baud (ví dụ 9600/8N2 -> ~4010us)

#define T35_US       1750    // ví dụ fixed cho baud cao

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
	NVIC_SetPriority(TIM4_IRQn, 6);
	NVIC_EnableIRQ(TIM4_IRQn);

	// Start
	TIM4->CR1 |= TIM_CR1_CEN;
}

void TIM4_init_T35(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    TIM4->CR1 = 0;
    TIM4->PSC = (TIM4CLK_HZ / 1000000UL) - 1;  // 45MHz -> 1MHz => PSC=44
    TIM4->ARR = T35_US - 1;                    // 1750us -> ARR=1749

    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR  = 0;
    TIM4->DIER |= TIM_DIER_UIE;

    TIM4->CR1 |= TIM_CR1_OPM;                  // one-shot (rất hợp Modbus RTU)

    NVIC_SetPriority(TIM4_IRQn, 5);
    NVIC_EnableIRQ(TIM4_IRQn);

    TIM4->CR1 &= ~TIM_CR1_CEN;                 // KHÔNG start ở init
}

