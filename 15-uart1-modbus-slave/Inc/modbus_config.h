/*
 * modbus_config.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_CONFIG_H_
#define MODBUS_CONFIG_H_

#pragma once
#include <stdint.h>

/* ===== Modbus RTU Slave Config ===== */
#define MB_SLAVE_ADDRESS        0x01

/* Với app hiện tại bạn chỉ đọc 1 input register ở địa chỉ 0x0001 */
#define MB_INPUT_REGISTER_ADDR  0x0001

/* RTU maximum frame */
#define MB_RX_MAX               256

/* Debug/Watchdog */
#define MB_FRAME_STUCK_TIMEOUT_MS  50u   /* drop partial frame nếu không có byte mới */

/* Exception codes */
#define MB_EX_ILLEGAL_FUNCTION      0x01
#define MB_EX_ILLEGAL_DATA_ADDR     0x02
#define MB_EX_ILLEGAL_DATA_VALUE    0x03
#define MB_EX_SLAVE_DEVICE_FAILURE  0x04

#endif /* MODBUS_CONFIG_H_ */
