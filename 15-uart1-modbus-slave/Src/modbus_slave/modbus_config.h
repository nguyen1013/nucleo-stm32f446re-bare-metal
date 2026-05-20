/*
 * modbus_config.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_CONFIG_H
#define MODBUS_CONFIG_H

#include <stdint.h>

/* ========= MODBUS CONFIG ========= */
#define SLAVE_ADDRESS 0x01
#define INPUT_REGISTER 0x01

/* Exception codes */
#define MB_EX_ILLEGAL_FUNCTION     0x01
#define MB_EX_ILLEGAL_DATA_ADDR    0x02
#define MB_EX_ILLEGAL_DATA_VALUE   0x03
#define MB_EX_SLAVE_DEVICE_FAILURE 0x04

/* Buffer */
#define RX_MAX 256

/* Timeout debug */
#define FRAME_STUCK_TIMEOUT_MS 50

#endif
