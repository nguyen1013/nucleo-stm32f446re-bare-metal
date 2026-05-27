/*
 * tim2.h
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#ifndef TIM2_H_
#define TIM2_H_

#include "stm32f4xx.h"

void Tim2_init(void);
void delay_us_tim2(uint32_t us);

#endif /* TIM2_H_ */
