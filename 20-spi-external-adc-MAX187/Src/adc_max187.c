/*
 * adc_max187.c
 *
 *  Created on: 28.5.2026
 *      Author: nn
 */

#include "stm32f4xx.h"
#include "spi.h"

/* ====== CONFIG ====== */

#define MAX187_CS_PORT   GPIOB
#define MAX187_CS_PIN    6

#define MAX187_VREF      4096   // Internal reference mV

/* ====== LOW-LEVEL CONTROL ====== */

#define MAX187_CS_LOW()   (MAX187_CS_PORT->BSRR = (1U << MAX187_CS_PIN) << 16)
#define MAX187_CS_HIGH()  (MAX187_CS_PORT->BSRR = (1U << MAX187_CS_PIN))

extern void delay_us_tim2(uint32_t us);

/* ====== INIT ====== */

void MAX187_Init(void) {
	// Enable GPIOB clock
	RCC->AHB1ENR |= (1U << 1);

	// PB6 output
	MAX187_CS_PORT->MODER &= ~(3U << (MAX187_CS_PIN * 2));
	MAX187_CS_PORT->MODER |= (1U << (MAX187_CS_PIN * 2));

	// High speed
	MAX187_CS_PORT->OSPEEDR |= (3U << (MAX187_CS_PIN * 2));

	// Default HIGH
	MAX187_CS_HIGH();
}

/* ====== READ ADC ====== */

uint16_t MAX187_Read(void) {
	uint8_t rx[2] = { 0 };
	uint8_t tx[2] = { 0xFF, 0xFF };

	uint16_t result;

	// 1. Start conversion
	MAX187_CS_LOW();

	// 2. Wait conversion (10us)
	delay_us_tim2(10);

	// 3. Read 16-bit (clock out data)
	SPI1_TransmitReceive(tx, rx, 2);

	// 4. Stop
	MAX187_CS_HIGH();

	// 5. Combine data
	result = (rx[0] << 8) | rx[1];

	/*
	 Bit format:
	 [15]      = EOC
	 [14..3]   = DATA (12-bit)
	 [2..0]    = padding
	 */

	result = (result >> 3) & 0x0FFF;

	return result;
}

/* ====== READ VOLTAGE ====== */

uint16_t MAX187_ReadVoltage_mV(void) {
	uint16_t adc = MAX187_Read();

	return (adc * MAX187_VREF) / 4095;
}
