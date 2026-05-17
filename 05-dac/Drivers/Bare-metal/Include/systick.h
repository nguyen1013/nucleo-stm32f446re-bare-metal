/*
 * systick.h
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "stm32f4xx.h"

void systick_delay_ms(uint32_t delay);
void systick_delay_s(uint32_t s);

#endif /* SYSTICK_H_ */
