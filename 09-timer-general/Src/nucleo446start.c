/*
 * nucleo446start.c
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#include "nucleo446start.h"

/* =========================================================
 *  Private helpers – bit-field shortcuts
 * ========================================================= */
#define RCC_CR_HSEON_BIT        16
#define RCC_CR_HSERDY_BIT       17
#define RCC_CR_PLLON_BIT        24
#define RCC_CR_PLLRDY_BIT       25

#define RCC_APB1ENR_PWREN_BIT   28

#define PWR_CR_VOS_SCALE1       (3UL << 14)  /* VOS = "Scale 1" for 180 MHz */
#define PWR_CR_ODEN_BIT         16
#define PWR_CR_ODSWEN_BIT       17
#define PWR_CSR_ODRDY_BIT       16
#define PWR_CSR_ODSWRDY_BIT     17

/* Flash ACR: PRFTEN | ICEN | DCEN | LATENCY(5 WS) */
#define FLASH_ACR_INIT          ((1 << 8) | (1 << 9) | (1 << 10) | (5 << 0))

#define PLL_SRC_HSE             (1UL << 22)

/* =========================================================
 *  SystemClock_Config
 * ========================================================= */
void SystemClock_Config(void) {
	/* 1. Enable HSE and wait until ready */
	RCC->CR |= (1UL << RCC_CR_HSEON_BIT);
	while (!(RCC->CR & (1UL << RCC_CR_HSERDY_BIT)))
		;

	/* 2. Enable Power interface clock; set VOS Scale 1 (required for 180 MHz) */
	RCC->APB1ENR |= (1UL << RCC_APB1ENR_PWREN_BIT);
	PWR->CR |= PWR_CR_VOS_SCALE1;

	/* 3. Enable Over-Drive mode (mandatory above 168 MHz on F446) */
	PWR->CR |= (1UL << PWR_CR_ODEN_BIT);
	while (!(PWR->CSR & (1UL << PWR_CSR_ODRDY_BIT)))
		;

	PWR->CR |= (1UL << PWR_CR_ODSWEN_BIT);
	while (!(PWR->CSR & (1UL << PWR_CSR_ODSWRDY_BIT)))
		;

	/* 4. Configure Flash: prefetch + I-cache + D-cache + 5 wait-states */
	FLASH->ACR = FLASH_ACR_INIT;

	/* 5. Configure bus prescalers
	 *    AHB = SYSCLK/1 = 180 MHz
	 *    APB1 = SYSCLK/4 =  45 MHz  (max 45 MHz)
	 *    APB2 = SYSCLK/2 =  90 MHz  (max 90 MHz) */
	RCC->CFGR &= ~(0xFUL << 4); /* Clear HPRE  → AHB /1  */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; /* APB1 /4  */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; /* APB2 /2  */

	/* 6. Configure Main PLL: source = HSE */
	RCC->PLLCFGR = ((uint32_t) PLL_M << 0) | ((uint32_t) PLL_N << 6)
			| ((uint32_t) PLL_P << 16) | PLL_SRC_HSE | ((uint32_t) PLL_Q << 24);

	/* 7. Enable PLL and wait until locked */
	RCC->CR |= (1UL << RCC_CR_PLLON_BIT);
	while (!(RCC->CR & (1UL << RCC_CR_PLLRDY_BIT)))
		;

	/* 8. Switch SYSCLK source to PLL and wait for hardware confirmation */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL))
		;
}

/* =========================================================
 *  fpu_enable
 * ========================================================= */
void fpu_enable(void) {
	/* Grant full access to CP10 (single-precision) and CP11 (double-precision) */
	SCB->CPACR |= ((3UL << (10 * 2)) | (3UL << (11 * 2)));

	/* Ensure the write completes before any FP instruction is fetched */
	__DSB();
	__ISB();
}

