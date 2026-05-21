/*
 * mb_fc04.c
 *
 *  Created on: 21.5.2026
 *      Author: nn
 */

#include "modbus_slave.h"
#include "modbus_crc.h"
#include "modbus_port_stm32.h"

/* -------- local helpers (private to FC04 handler) -------- */

static void mb_send_bytes(const uint8_t *buf, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		Modbus_Send_Byte(buf[i]);
	}
}

static void mb_send_exception(uint8_t req_func, uint8_t ex_code) {
	uint8_t tx[5];

	tx[0] = SLAVE_ADDRESS;
	tx[1] = (uint8_t) (req_func | 0x80);
	tx[2] = ex_code;

	uint16_t crc = CRC16((char*) tx, 3);
	tx[3] = (uint8_t) (crc & 0xFF);
	tx[4] = (uint8_t) ((crc >> 8) & 0xFF);

	mb_send_bytes(tx, 5);
	mb_dbg_exc_sent++;
}

static void mb_send_fc04_response(uint16_t qty, const uint16_t *regs) {
	/* Response: [addr][04][byteCount][data...][CRC] */
	/* byteCount = 2*qty */
	static uint8_t tx[RX_MAX];

	uint16_t p = 0;
	uint16_t byteCount = (uint16_t) (qty * 2U);

	tx[p++] = SLAVE_ADDRESS;
	tx[p++] = MB_FC_READ_INPUT_REGS;
	tx[p++] = (uint8_t) byteCount;

	for (uint16_t i = 0; i < qty; i++) {
		tx[p++] = (uint8_t) (regs[i] >> 8);
		tx[p++] = (uint8_t) (regs[i] & 0xFF);
	}

	uint16_t crc = CRC16((char*) tx, (int) p);
	tx[p++] = (uint8_t) (crc & 0xFF);
	tx[p++] = (uint8_t) ((crc >> 8) & 0xFF);

	mb_send_bytes(tx, p);
}

/* ================== Weak default callbacks ==================
 * If not implement => handler will return exception 0x04.
 */
int __attribute__((weak)) MB_App_ReadInputRegisters(uint16_t addr, uint16_t qty,
		uint16_t *outBuf) {
	(void) addr;
	(void) qty;
	(void) outBuf;
	return -1;
}

/* ================== FC04 handler ================== */
int MB_FC04_ReadInputRegisters(const uint8_t *frame, uint16_t len) {
	/* Request FC04 RTU length fixed: 8 bytes */
	if (len != MB_RTU_REQ_FC04_LEN) {
		mb_send_exception(MB_FC_READ_INPUT_REGS, MB_EX_ILLEGAL_DATA_VALUE);
		return -1;
	}

	uint16_t start_addr = (uint16_t) ((frame[2] << 8) | frame[3]);
	uint16_t qty = (uint16_t) ((frame[4] << 8) | frame[5]);

	if (qty < 1 || qty > MB_RTU_MAX_REGS) {
		mb_send_exception(MB_FC_READ_INPUT_REGS, MB_EX_ILLEGAL_DATA_VALUE);
		return -1;
	}

	/* Address range check based on config mapping */
	if (start_addr < MB_INPUT_START_ADDR
			|| (uint32_t) start_addr + qty - 1U
					>= (uint32_t) MB_INPUT_START_ADDR + MB_INPUT_NUM_REGS) {
		mb_send_exception(MB_FC_READ_INPUT_REGS, MB_EX_ILLEGAL_DATA_ADDR);
		return -1;
	}

	uint16_t regs[MB_RTU_MAX_REGS];

	/* Prefer multi-register callback */
	if (MB_App_ReadInputRegisters(start_addr, qty, regs) != 0) {

		mb_send_exception(MB_FC_READ_INPUT_REGS, MB_EX_SLAVE_DEVICE_FAILURE);
		return -1;
	}

	mb_send_fc04_response(qty, regs);
	mb_dbg_ok++;
	return 0;
}
