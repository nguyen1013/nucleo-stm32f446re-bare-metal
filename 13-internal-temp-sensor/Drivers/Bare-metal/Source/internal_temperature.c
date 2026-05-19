/*
 * internal_temperature.c
 *
 *  Created on: 19.5.2026
 *      Author: nn
 */

#include "internal_temperature.h"

/* ---- Factory calibration addresses (STM32F446 datasheet Table 81) ---- */
#define TS_CAL1_ADDR (0x1FFF7A2Cu)  /* 30°C, VDDA=3.3V */
#define TS_CAL2_ADDR (0x1FFF7A2Eu)  /* 110°C, VDDA=3.3V */

#define TS_CAL1 (*((uint16_t*)TS_CAL1_ADDR))
#define TS_CAL2 (*((uint16_t*)TS_CAL2_ADDR))

/* Calibration temperatures from datasheet */
#define TS_T1_C (30.0f)
#define TS_T2_C (110.0f)

/* Small delay to allow the temp sensor to start up after enabling TSVREFE.
 Datasheet says typ 6 µs, max 10 µs. */
static void delay_cycles(volatile uint32_t cycles) {
	while (cycles--) {
		__NOP();
	}
}

static void temp_sensor_startup_delay(void) {
	/* Rough delay: assumes ~100+ MHz core clock -> a few hundred cycles ~ a few µs */
	delay_cycles(2000);
}

void InternalTemp_Init(void) {
	/* Enable ADC1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/* Enable temperature sensor & VREFINT */
	ADC->CCR |= ADC_CCR_TSVREFE;

	/* Wait for sensor startup */
	temp_sensor_startup_delay();

	/* ADC prescaler: keep ADC clock within spec (common safe choice: PCLK2/4). */
	ADC->CCR &= ~(ADC_CCR_ADCPRE);
	ADC->CCR |= (ADC_CCR_ADCPRE_0); /* /4 */

	/* 12-bit resolution, single conversion, software trigger */
	ADC1->CR1 &= ~(ADC_CR1_RES);
	ADC1->CR2 &= ~(ADC_CR2_CONT);
	ADC1->CR2 &= ~(ADC_CR2_EXTEN);

	/* Regular sequence length = 1 conversion */
	ADC1->SQR1 &= ~(ADC_SQR1_L);

	/* 1st conversion = channel 16 (temperature sensor is ADC1_IN16 on STM32F4) */
	ADC1->SQR3 &= ~(ADC_SQR3_SQ1);
	ADC1->SQR3 |= (16u << ADC_SQR3_SQ1_Pos);

	/* Sampling time: MUST be >= 10 µs for ~1°C accuracy (datasheet Table 80).
	 Set 480 cycles (all bits = 1). */
	ADC1->SMPR1 &= ~(ADC_SMPR1_SMP16);
	ADC1->SMPR1 |= (ADC_SMPR1_SMP16_0 | ADC_SMPR1_SMP16_1 | ADC_SMPR1_SMP16_2);

	/* Enable ADC */
	ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t InternalTemp_ReadRaw(void) {
	/* Start conversion */
	ADC1->CR2 |= ADC_CR2_SWSTART;

	/* Wait end of conversion */
	while (!(ADC1->SR & ADC_SR_EOC)) {
	}

	return (uint16_t) (ADC1->DR & 0xFFFFu);
}

float InternalTemp_ConvertToC_Calibrated(uint16_t raw_adc) {
	const float cal1 = (float) TS_CAL1;
	const float cal2 = (float) TS_CAL2;
	const float raw = (float) raw_adc;

	/* Linear interpolation between (cal1 @ 30°C) and (cal2 @ 110°C) */
	return ((raw - cal1) * (TS_T2_C - TS_T1_C) / (cal2 - cal1)) + TS_T1_C;
}

float InternalTemp_ReadC(void) {
	return InternalTemp_ConvertToC_Calibrated(InternalTemp_ReadRaw());
}
