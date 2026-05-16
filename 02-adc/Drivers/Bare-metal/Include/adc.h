/*
 * adc.h
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#ifndef ADC_H
#define ADC_H

#include "nucleo446start.h"

/************** PUBLIC FUNCTION PROTOTYPES *****************
 * ADC_Init        : Configure ADC1 (GPIO, prescaler, scan mode, resolution,
 *                   continuous conversion, sampling time, channel sequence)
 * ADC_Enable      : Power on ADC1 and wait for stabilization (~10us)
 * ADC_Start       : Select channel, clear status register, trigger conversion
 * ADC_WaitForConv : Block until EOC flag is set
 * ADC_GetVal      : Read and return the 12-bit result from DR
 * ADC_Disable     : Power off ADC1
 ***********************************************************/

void ADC_Init(void);
void ADC_Enable(void);
void ADC_Start(int channel);
void ADC_WaitForConv(void);
uint16_t ADC_GetVal(void);
void ADC_Disable(void);

#endif /* ADC_H */
