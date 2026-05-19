/*
 * internal_temperature.h
 *
 *  Created on: 19.5.2026
 *      Author: nn
 */

#ifndef INTERNAL_TEMPERATURE_H
#define INTERNAL_TEMPERATURE_H

#include "stm32f446xx.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialise ADC1 for internal temperature sensor (channel 16). */
void InternalTemp_Init(void);

/** Read raw ADC (12-bit) value of temperature sensor channel. */
uint16_t InternalTemp_ReadRaw(void);

/** Convert raw ADC to °C using factory calibration points (30°C & 110°C). */
float InternalTemp_ConvertToC_Calibrated(uint16_t raw_adc);

/** Convenience: read temperature in °C using factory calibration. */
float InternalTemp_ReadC(void);

#ifdef __cplusplus
}
#endif

#endif /* INTERNAL_TEMPERATURE_H */
