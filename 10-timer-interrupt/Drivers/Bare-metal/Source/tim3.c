/*
 * tim3.c
 *
 *  Created on: 18.5.2026
 *      Author: nn
 */

#include "tim3.h"

void Tim3_init(void)
{
    // Enable clock TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Prescaler & ARR (1s)
    TIM3->PSC = 8999;     // 90MHz / 9000 = 10kHz
    TIM3->ARR = 9999;     // 10kHz / 10000 = 1Hz

    // Update register
    TIM3->EGR = TIM_EGR_UG;

    // Clear flag
    TIM3->SR &= ~TIM_SR_UIF;

    // Enable interrupt
    TIM3->DIER |= TIM_DIER_UIE;

    // NVIC
    NVIC_EnableIRQ(TIM3_IRQn);

    // Start timer
    TIM3->CR1 |= TIM_CR1_CEN;
}

