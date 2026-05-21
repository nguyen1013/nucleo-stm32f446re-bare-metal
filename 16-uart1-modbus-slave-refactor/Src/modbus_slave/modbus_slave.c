/*
 * modbus_slave.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "modbus_slave.h"
#include "modbus_crc.h"
#include "modbus_port_stm32.h"

/* ================== Debug counters ================== */
volatile uint32_t mb_dbg_crc_fail = 0;
volatile uint32_t mb_dbg_exc_sent = 0;
volatile uint32_t mb_dbg_ignored = 0;
volatile uint32_t mb_dbg_ok = 0;

/* ================== Local helper: send exception ==================
 * Core layer needs to send ILLEGAL_FUNCTION when no handler exists.
 * CRC wrong / wrong address => silent drop.
 */
static void mb_send_exception_core(uint8_t req_func, uint8_t ex_code) {
	/* Exception response: [addr][func|0x80][ex][CRClo][CRChi] */
	uint8_t tx[5];

	tx[0] = SLAVE_ADDRESS;
	tx[1] = (uint8_t) (req_func | 0x80);
	tx[2] = ex_code;

	uint16_t crc = CRC16((char*) tx, 3);
	tx[3] = (uint8_t) (crc & 0xFF);
	tx[4] = (uint8_t) ((crc >> 8) & 0xFF);

	/* Send via port */
	for (int i = 0; i < 5; i++) {
		Modbus_Send_Byte(tx[i]);
	}

	mb_dbg_exc_sent++;
}

/* ================== Dispatch table ================== */
typedef struct {
	uint8_t func;
	mb_func_handler_t handler;
} mb_func_entry_t;

static const mb_func_entry_t mb_func_table[] = { { MB_FC_READ_INPUT_REGS,
		MB_FC04_ReadInputRegisters }, { MB_FC_READ_HOLDING_REGS,
		MB_FC03_ReadHoldingRegisters }, { MB_FC_WRITE_SINGLE_REG,
		MB_FC06_WriteSingleRegister }, { MB_FC_WRITE_MULTI_REGS,
		MB_FC16_WriteMultipleRegisters }, };

static mb_func_handler_t mb_find_handler(uint8_t func) {
	for (uint32_t i = 0;
			i < (uint32_t) (sizeof(mb_func_table) / sizeof(mb_func_table[0]));
			i++) {
		if (mb_func_table[i].func == func)
			return mb_func_table[i].handler;
	}
	return (mb_func_handler_t) 0;
}

/* ================== Public API ================== */
void MB_Slave_ProcessFrame(const uint8_t *frame, uint16_t len) {
	/* 1) Minimum length (must have CRC) */
	if (len < MB_RTU_MIN_LEN) {
		mb_dbg_ignored++;
		return;
	}

	/* 2) Address check */
	if (frame[0] != SLAVE_ADDRESS) {
		mb_dbg_ignored++;
		return;
	}

	/* 3) CRC check (last 2 bytes are CRC lo/hi) */
	uint16_t calc_crc = CRC16((char*) frame, (int) (len - 2));
	uint8_t crc_lo = (uint8_t) (calc_crc & 0xFF);
	uint8_t crc_hi = (uint8_t) ((calc_crc >> 8) & 0xFF);

	if (frame[len - 2] != crc_lo || frame[len - 1] != crc_hi) {
		mb_dbg_crc_fail++;
		return; /* Modbus RTU: CRC wrong => silent drop */
	}

	/* 4) Dispatch to handler */
	uint8_t func = frame[1];
	mb_func_handler_t h = mb_find_handler(func);

	if (!h) {
		mb_send_exception_core(func, MB_EX_ILLEGAL_FUNCTION);
		return;
	}

	(void) h(frame, len);
}

/* =========================================================
 * Weak stubs (so project builds even before you implement
 * FC03/FC06/FC16 handler files). Later, create handler files
 * to override these.
 * ========================================================= */

int __attribute__((weak)) MB_FC03_ReadHoldingRegisters(const uint8_t *frame,
		uint16_t len) {
	(void) frame;
	(void) len;
	mb_send_exception_core(MB_FC_READ_HOLDING_REGS, MB_EX_ILLEGAL_FUNCTION);
	return -1;
}

int __attribute__((weak)) MB_FC06_WriteSingleRegister(const uint8_t *frame,
		uint16_t len) {
	(void) frame;
	(void) len;
	mb_send_exception_core(MB_FC_WRITE_SINGLE_REG, MB_EX_ILLEGAL_FUNCTION);
	return -1;
}

int __attribute__((weak)) MB_FC16_WriteMultipleRegisters(const uint8_t *frame,
		uint16_t len) {
	(void) frame;
	(void) len;
	mb_send_exception_core(MB_FC_WRITE_MULTI_REGS, MB_EX_ILLEGAL_FUNCTION);
	return -1;
}
