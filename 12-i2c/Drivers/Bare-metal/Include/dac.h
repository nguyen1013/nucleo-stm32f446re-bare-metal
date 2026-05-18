/*
 * dac1.h
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#ifndef DAC_H_
#define DAC_H_

#include "nucleo446start.h"

void DAC_Init_PA4(void);
void DAC_Write_PA4(uint16_t value);

#endif /* DAC_H_ */
