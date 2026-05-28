/*
 * adc_max187.h
 *
 *  Created on: 28.5.2026
 *      Author: nn
 */

#ifndef ADC_MAX187_H_
#define ADC_MAX187_H_

#include <stdint.h>

/* ====== Public API ====== */

void MAX187_Init(void);

// Read 1 value ADC (12-bit)
uint16_t MAX187_Read(void);

// Convert to (miliVolt)
uint16_t MAX187_ReadVoltage_mV(void);

#endif /* ADC_MAX187_H_ */
