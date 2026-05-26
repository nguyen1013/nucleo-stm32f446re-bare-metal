/*
 * I2C1_Write_LCD.c
 *
 *  Created on: 25.5.2026
 *      Author: nn
 */

#include "stm32f4xx.h"
#include "tim6.h"

extern void I2C_Start(void);
extern void I2C_Address(uint8_t Address);
extern void I2C_WriteMulti(uint8_t *data, uint8_t size);
extern void I2C_Stop(void);

// Timeout waiting loop BUSY/release bus (tuning depend on system)
#ifndef I2C_TIMEOUT_TICKS
#define I2C_TIMEOUT_TICKS   (300000UL)
#endif

static inline int wait_bus_not_busy(uint32_t to) {
	while (I2C1->SR2 & I2C_SR2_BUSY) {
		if (to-- == 0U)
			return -1;
	}
	return 0;
}

static inline int has_i2c_error(void) {
	uint32_t sr1 = I2C1->SR1;
	return ((sr1
			& (I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_OVR
					| I2C_SR1_TIMEOUT)) != 0U);
}

static inline void clear_i2c_errors(void) {
	// Clear common error flags by writing 0 to them
	I2C1->SR1 &= ~(I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_OVR
			| I2C_SR1_TIMEOUT);
}

static inline void i2c_soft_reset_recover(void) {
	// Soft reset I2C peripheral to recover from stuck BUSY/state machine
	I2C1->CR1 |= (1U << 15);   // SWRST = 1

	delay_ms_tim6(1);
	I2C1->CR1 &= ~(1U << 15);   // SWRST = 0

	// Re-enable peripheral (incase reset making clear PE)
	I2C1->CR1 |= (1U << 0);     // PE = 1
}

/**
 * Write n bytes to I2C (LCD via PCF8574).
 * address: 7-bit
 * n: number of bytes
 * data: buffer
 *
 * return: 0 OK, <0 error
 */
int I2C1_Write_LCD(uint8_t address, int n, uint8_t *data) {
	if (!data || n <= 0)
		return -1;

	// Use static flag to avoid repeating init.
	static uint8_t tim6_ready = 0;
	if (!tim6_ready) {
		Tim6_init();
		tim6_ready = 1;
	}

	// 1) Wait for free bus(timeout). If stuck -> try recover by SWRST.
	if (wait_bus_not_busy(I2C_TIMEOUT_TICKS) < 0) {
		i2c_soft_reset_recover();
		if (wait_bus_not_busy(I2C_TIMEOUT_TICKS) < 0) {
			return -2; // bus still busy
		}
	}

	// 2) Clear previous errors (avoid “AF/BERR” since last time)
	clear_i2c_errors();

	// 3) START
	I2C_Start();

	// 4) Convert 7-bit -> 8-bit (write)
	uint8_t addr_write = (uint8_t) ((address << 1) & 0xFE);  // R/W=0
	I2C_Address(addr_write);

	// 5) If error after address (AF=NACK...) -> STOP + clear
	if (has_i2c_error()) {
		I2C_Stop();
		clear_i2c_errors();
		// small delay for bus settle
		delay_ms_tim6(1);
		return -3;
	}

	// 6) Send multi bytes
	I2C_WriteMulti(data, (uint8_t) n);

	// 7) STOP
	I2C_Stop();

	// 8) Delay bus settle
	delay_ms_tim6(1);

	// 9) Check error after transmission
	if (has_i2c_error()) {
		clear_i2c_errors();

		// If bus busy, try recover
		if (I2C1->SR2 & I2C_SR2_BUSY) {
			i2c_soft_reset_recover();
		}
		return -4;
	}

	// 10) wait for BUSY to 0 to ensure STOP released bus (timeout)
	if (wait_bus_not_busy(I2C_TIMEOUT_TICKS) < 0) {
		i2c_soft_reset_recover();
		if (wait_bus_not_busy(I2C_TIMEOUT_TICKS) < 0) {
			return -5;
		}
	}

	return 0;
}
