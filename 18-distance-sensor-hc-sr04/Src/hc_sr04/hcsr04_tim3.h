/*
 * hcsr04_tim3.h
 *
 *  Created on: 26.5.2026
 *      Author: nn
 */

#ifndef HCSR04_TIM3_H_
#define HCSR04_TIM3_H_

#include "stm32f4xx.h"

void HCSR04_TIM3_Init(void);
uint32_t HCSR04_TIM3_ReadPulse_us(uint32_t timeout_us);
uint32_t HCSR04_TIM3_ReadDistance_cm(uint32_t timeout_us);

#endif /* HCSR04_TIM3_H_ */

