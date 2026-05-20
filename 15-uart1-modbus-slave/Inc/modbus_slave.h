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

/* ================== Debug counters (optional) ================== */
extern volatile uint32_t mb_dbg_crc_fail;
extern volatile uint32_t mb_dbg_exc_sent;
extern volatile uint32_t mb_dbg_ignored; /* not my addr, too short... */
extern volatile uint32_t mb_dbg_ok;

/* ================== Public API ================== */

/**
 * Process a frame Modbus RTU (received a full frame).
 * - Self check: len, slave address, CRC
 * - Support FC04 (depend on configuration)
 * - Self send response/exception via Modbus_Send_Byte()
 */
void MB_Slave_ProcessFrame(const uint8_t *frame, uint16_t len);

/* ================== Application callbacks ==================
 * Implement these function in app layer
 * Otherwise, module return exception DEVICE FAILURE.
 */

/**
 * Read Input Register at addr.
 * Return:
 *   0  : OK, *outValue valid
 *  !=0 : reading error (return exception 0x04)
 */
int MB_App_ReadInputRegister(uint16_t addr, uint16_t *outValue);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_SLAVE_H */
