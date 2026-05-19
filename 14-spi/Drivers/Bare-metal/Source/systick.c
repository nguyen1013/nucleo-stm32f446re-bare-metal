/*
 * systick.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */


#include "systick.h"

#define SYSTICK_LOAD_VALUE 			22500000 // CLKSOURCE: HCLK/8 = 180Mhz/8 =  22.5 MHz

void systick_delay_ms(uint32_t ms)
{
    uint32_t ticks_per_ms = SYSTICK_LOAD_VALUE / 1000U;

    // ticks_per_ms must fit in 24-bit reload register (max 0xFFFFFF)
    if (ticks_per_ms == 0U || ticks_per_ms > 0xFFFFFFU) {
        // clock too low or too high for 1ms with SysTick reload
        return;
    }

    SysTick->LOAD = ticks_per_ms - 1U;
    SysTick->VAL  = 0U;

    // ENABLE + (optional) CLKSOURCE already set/cleared by configuration
    SysTick->CTRL = (SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) | SysTick_CTRL_ENABLE_Msk;

    while (ms--) {
        // COUNTFLAG set when reaches 0; reading clears it.
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0U) { }
    }

    SysTick->CTRL = 0U;
}

void systick_delay_s(uint32_t s)
{
    while (s--) {
        systick_delay_ms(1000);
    }
}

