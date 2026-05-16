/*
 * timer.h
 *
 *  Created on: 15.5.2026
 *      Author: nn
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f4xx.h"


void Tim6_init(void);

void delay_ms(uint32_t ms);

#endif /* TIMER_H_ */
