/*
 * modbus_port_stm32.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_PORT_STM32_H_
#define MODBUS_PORT_STM32_H_

#pragma once
#include <stdint.h>
#include <stdbool.h>

/* Platform primitives */
uint32_t mb_port_millis(void);

/* UART send (Modbus line) */
void mb_port_send_bytes(const uint8_t *data, uint16_t len);

/* Debug log (UART2) */
void mb_port_dbg_puts(const char *s);
void mb_port_dbg_hexdump(const uint8_t *buf, uint16_t len);

/* Critical section helpers */
void mb_port_enter_crit(void);
void mb_port_exit_crit(void);

/* T3.5 timer control (TIM4) */
void mb_port_t35_reset(void);

#endif /* MODBUS_PORT_STM32_H_ */
