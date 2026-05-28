/*
 * timer_dwt.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "timer_dwt.h"

#ifndef CPU_HZ
#define CPU_HZ 180000000UL
#endif

void Dwt_delay_init(void) {
	// Enable trace & debug block (TRCENA) to access DWT
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	// Reset cycle counter
	DWT->CYCCNT = 0;

	// Enable cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline void delay_cycles(uint32_t cycles) {
	uint32_t start = DWT->CYCCNT;
	while ((uint32_t) (DWT->CYCCNT - start) < cycles) {
		__asm volatile ("nop");
	}
}

void dwt_delay_us(uint32_t us) {
	// 180MHz -> 180 cycles/us
	const uint32_t cycles_per_us = (CPU_HZ / 1000000UL);
	delay_cycles(us * cycles_per_us);
}

void dwt_delay_ms(uint32_t ms) {
	const uint32_t cycles_per_ms = CPU_HZ / 1000UL;

	while (ms--) {
		uint32_t start = DWT->CYCCNT;
		while ((uint32_t) (DWT->CYCCNT - start) < cycles_per_ms) {
			__asm volatile ("nop");
		}
	}
}

