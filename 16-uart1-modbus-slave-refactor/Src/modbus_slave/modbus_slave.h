/*
 * modbus_slave.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include <stdint.h>
#include "modbus_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================
 *                DEBUG COUNTERS
 * ========================================================= */
extern volatile uint32_t mb_dbg_crc_fail;
extern volatile uint32_t mb_dbg_exc_sent;
extern volatile uint32_t mb_dbg_ignored;
extern volatile uint32_t mb_dbg_ok;

/* =========================================================
 *                FUNCTION HANDLER TYPE (DISPATCH)
 * ========================================================= */
typedef int (*mb_func_handler_t)(const uint8_t *frame, uint16_t len);

/* =========================================================
 *                PUBLIC API
 * ========================================================= */
/**
 * Process a Modbus RTU frame.
 * Responsibilities:
 *  - Validate frame length
 *  - Check slave address
 *  - Verify CRC
 *  - Dispatch to appropriate function handler
 *  - Send response or exception
 */
void MB_Slave_ProcessFrame(const uint8_t *frame, uint16_t len);

/* =========================================================
 *                FUNCTION HANDLERS (MODBUS LAYER)
 * ========================================================= */
/* FC04: Read Input Registers */
int MB_FC04_ReadInputRegisters(const uint8_t *frame, uint16_t len);

/* FC03: Read Holding Registers */
int MB_FC03_ReadHoldingRegisters(const uint8_t *frame, uint16_t len);

/* FC06: Write Single Register */
int MB_FC06_WriteSingleRegister(const uint8_t *frame, uint16_t len);

/* FC16: Write Multiple Registers */
int MB_FC16_WriteMultipleRegisters(const uint8_t *frame, uint16_t len);

/* =========================================================
 *                APPLICATION CALLBACK API
 * ========================================================= */
/**
 * Read multiple input registers
 */
int MB_App_ReadInputRegisters(uint16_t addr, uint16_t qty, uint16_t *outBuf);

/**
 * Read multiple holding registers
 */
int MB_App_ReadHoldingRegisters(uint16_t addr, uint16_t qty, uint16_t *outBuf);

/**
 * Write single holding register
 */
int MB_App_WriteHoldingRegister(uint16_t addr, uint16_t value);

/**
 * Write multiple holding registers
 */
int MB_App_WriteHoldingRegisters(uint16_t addr, uint16_t qty,
		const uint16_t *data);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_SLAVE_H */
