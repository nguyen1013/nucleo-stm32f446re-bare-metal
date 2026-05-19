/*
 * timer_dwt.h
 *
 *  Created on: 17.5.2026
 *      Author: nn
 *
 * NOTE: dwt takes control CPU 100%
 */

#ifndef TIMER_DWT_H_
#define TIMER_DWT_H_

#include "stm32f4xx.h"

void Dwt_delay_init(void);
void dwt_delay_us(uint32_t us);
void dwt_delay_ms(uint32_t ms);

#endif /* TIMER_DWT_H_ */
