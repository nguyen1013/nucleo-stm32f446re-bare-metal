/*
 * timer.c
 *
 *  Created on: 15.5.2026
 *      Author: nn
 */

#include "timer.h"


void tim6_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->PSC = 8999;   // 90 MHz / (8999+1) = 10 kHz
    TIM6->ARR = 9;      // 10 ticks = 1 ms

    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
}

void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        TIM6->SR &= ~TIM_SR_UIF;   // clear flag
        while (!(TIM6->SR & TIM_SR_UIF));  // wait overflow
    }
}
