/*
 * modbus_slave.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "modbus_slave.h"
#include "modbus_crc.h"
#include "modbus_port_stm32.h"   // Modbus_Send_Byte()

/* ================== Debug counters ================== */
volatile uint32_t mb_dbg_crc_fail = 0;
volatile uint32_t mb_dbg_exc_sent = 0;
volatile uint32_t mb_dbg_ignored = 0;
volatile uint32_t mb_dbg_ok = 0;

/* ================== Local helpers ================== */
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
	tx[3] = (uint8_t) (crc & 0xFF);         // CRC low
	tx[4] = (uint8_t) ((crc >> 8) & 0xFF);  // CRC high

	mb_send_bytes(tx, 5);
	mb_dbg_exc_sent++;
}

static void mb_send_fc04_response_uint16(uint16_t value) {
	uint8_t tx[7];
	tx[0] = SLAVE_ADDRESS;
	tx[1] = 0x04;
	tx[2] = 0x02;                 // byte count
	tx[3] = (uint8_t) (value >> 8);
	tx[4] = (uint8_t) (value & 0xFF);

	uint16_t crc = CRC16((char*) tx, 5);
	tx[5] = (uint8_t) (crc & 0xFF);
	tx[6] = (uint8_t) ((crc >> 8) & 0xFF);

	mb_send_bytes(tx, 7);
}

/* ================== Weak default callback (optional) ================== */
int __attribute__((weak)) MB_App_ReadInputRegister(uint16_t addr,
		uint16_t *outValue) {
	(void) addr;
	(void) outValue;
	return -1;
}

/* ================== Public API ================== */
void MB_Slave_ProcessFrame(const uint8_t *frame, uint16_t len) {
	/* 1) Basic length */
	if (len < 4) {
		mb_dbg_ignored++;
		return;
	}

	/* 2) Address check */
	if (frame[0] != SLAVE_ADDRESS) {
		mb_dbg_ignored++;

		return;
	}

	/* 3) CRC check: last 2 bytes = CRC lo/hi */
	if (len < 5) {
		mb_dbg_ignored++;
		return;
	}

	uint16_t calc_crc = CRC16((char*) frame, (int) (len - 2));
	uint8_t crc_lo = (uint8_t) (calc_crc & 0xFF);
	uint8_t crc_hi = (uint8_t) ((calc_crc >> 8) & 0xFF);

	if (frame[len - 2] != crc_lo || frame[len - 1] != crc_hi) {
		mb_dbg_crc_fail++;

		return; // Modbus RTU: if wrong CRC then silent (master timeout)
	}

	/* 4) Function code */
	uint8_t func = frame[1];

	switch (func) {
	case 0x04: {
		/* FC04 request RTU length must be 8:
		 [addr][04][startHi][startLo][qtyHi][qtyLo][crclo][crchi] */
		if (len != 8) {
			mb_send_exception(func, MB_EX_ILLEGAL_DATA_VALUE);
			return;
		}

		uint16_t start_addr = (uint16_t) ((frame[2] << 8) | frame[3]);
		uint16_t quantity = (uint16_t) ((frame[4] << 8) | frame[5]);

		if (quantity != 1) {
			mb_send_exception(func, MB_EX_ILLEGAL_DATA_VALUE);
			return;
		}

		if (start_addr != INPUT_REGISTER) {
			mb_send_exception(func, MB_EX_ILLEGAL_DATA_ADDR);
			return;
		}

		uint16_t value = 0;
		if (MB_App_ReadInputRegister(start_addr, &value) != 0) {
			mb_send_exception(func, MB_EX_SLAVE_DEVICE_FAILURE);
			return;
		}

		mb_send_fc04_response_uint16(value);
		mb_dbg_ok++;
		return;
	}

	default:
		mb_send_exception(func, MB_EX_ILLEGAL_FUNCTION);
		return;
	}
}

