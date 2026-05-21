/*
 * modbus_config.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_CONFIG_H
#define MODBUS_CONFIG_H

#include <stdint.h>

/* =========================================================
 *                MODBUS GENERAL CONFIG
 * ========================================================= */
#define SLAVE_ADDRESS      0x01

/* =========================================================
 *                MODBUS FUNCTION CODES
 * ========================================================= */
#define MB_FC_READ_COILS              0x01
#define MB_FC_READ_DISCRETE_INPUTS    0x02
#define MB_FC_READ_HOLDING_REGS       0x03
#define MB_FC_READ_INPUT_REGS         0x04
#define MB_FC_WRITE_SINGLE_REG        0x06
#define MB_FC_WRITE_MULTI_REGS        0x10

/* =========================================================
 *                MODBUS EXCEPTION CODES
 * ========================================================= */
#define MB_EX_ILLEGAL_FUNCTION     0x01
#define MB_EX_ILLEGAL_DATA_ADDR    0x02
#define MB_EX_ILLEGAL_DATA_VALUE   0x03
#define MB_EX_SLAVE_DEVICE_FAILURE 0x04

/* =========================================================
 *                FRAME / BUFFER CONFIG
 * ========================================================= */
#define RX_MAX             256        // maximum RTU frame size

/* =========================================================
 *                RTU FRAME STRUCTURE
 * ========================================================= */
#define MB_RTU_MIN_LEN     5          // addr + func + CRC(2)
#define MB_RTU_MAX_REGS    125        // theo Modbus spec

/* =========================================================
 *                FUNCTION-SPECIFIC LENGTH
 * ========================================================= */
#define MB_RTU_REQ_FC04_LEN   8       // read input register request
#define MB_RTU_REQ_FC03_LEN   8       // read holding register request
#define MB_RTU_REQ_FC06_LEN   8       // write single register
#define MB_RTU_REQ_FC16_MIN   9       // write multiple (min frame)

/* =========================================================
 *                REGISTER MAPPING (APPLICATION)
 * ========================================================= */
#define MB_INPUT_START_ADDR       0x0001
#define MB_INPUT_NUM_REGS         1

#define MB_HOLDING_START_ADDR     0x0001
#define MB_HOLDING_NUM_REGS       10

/* =========================================================
 *                DEBUG CONFIG
 * ========================================================= */
#define MB_DEBUG_ENABLE 0

#if MB_DEBUG_ENABLE
#define MB_LOG(x) DebugUART_SendString(x)
#else
#define MB_LOG(x)
#endif

#endif /* MODBUS_CONFIG_H */
