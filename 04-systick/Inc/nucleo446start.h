/*
 * nucleo446start.h
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#ifndef NUCLEO446START_H
#define NUCLEO446START_H

#include "stm32f4xx.h"

/* =========================================================
 *  PLL Configuration for STM32F446RE @ 180 MHz
 *
 *  Source      : HSE
 *  VCO input   : HSE / PLL_M  =  8 MHz / 4  =  2 MHz
 *  VCO output  : VCO_in * PLL_N = 2 * 180   = 360 MHz
 *  SYSCLK      : VCO_out / PLLP = 360 / 2   = 180 MHz
 *  USB/SDIO    : VCO_out / PLL_Q = 360 / 8  =  45 MHz
 * ========================================================= */
#define PLL_M   4       /* VCO input divider  (2–63)          */
#define PLL_N   180     /* VCO multiplier     (50–432)        */
#define PLL_P   0       /* SYSCLK divider: 0→/2, 1→/4 ...    */
#define PLL_Q   8       /* USB/SDIO/RNG clock = 360/8 = 45MHz */

/* =========================================================
 *  Public API
 * ========================================================= */

/**
 * @brief  Configure system clock to 180 MHz using HSE + PLL.
 *         - HSE as PLL source
 *         - Over-Drive mode enabled (required above 168 MHz)
 *         - Flash: prefetch + instruction/data cache, 5 wait-states
 *         - AHB  : /1  → 180 MHz
 *         - APB1 : /4  →  45 MHz
 *         - APB2 : /2  →  90 MHz
 */
void SystemClock_Config(void);

/**
 * @brief  Enable the Floating Point Unit (FPU).
 *         Grants full access to CP10 and CP11 coprocessors.
 *         Call before any floating-point operation.
 */
void fpu_enable(void);

#endif /* NUCLEO446START_H */

