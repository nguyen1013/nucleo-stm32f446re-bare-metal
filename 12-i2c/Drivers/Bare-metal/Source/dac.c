/*
 * dac1.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "dac.h"

void DAC_Init_PA4(void)
{
    // 1) Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 2) PA4 analog mode: MODER4 = 11 (clear then set)
    GPIOA->MODER &= ~(3U << (4U * 2U));
    GPIOA->MODER |=  (3U << (4U * 2U));

    // 3) No pull-up/pull-down on PA4
    GPIOA->PUPDR &= ~(3U << (4U * 2U));

    // 4) Enable DAC clock
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // 5) Disable CH1 before config (an toàn)
    DAC->CR &= ~DAC_CR_EN1;

    // 6) No trigger if you write by software
    DAC->CR &= ~DAC_CR_TEN1;

    // 7) Buffer mode:
    //    - Buffer ON  (recommended for easier drive/measure): BOFF1=0
    //    - Buffer OFF (high-Z): BOFF1=1
    DAC->CR &= ~DAC_CR_BOFF1;   // buffer ON

    // 8) Enable DAC channel 1
    DAC->CR |= DAC_CR_EN1;
}

void DAC_Write_PA4(uint16_t value)
{
    if (value > 4095) value = 4095;
    DAC->DHR12R1 = value;   // 12-bit right aligned
}
