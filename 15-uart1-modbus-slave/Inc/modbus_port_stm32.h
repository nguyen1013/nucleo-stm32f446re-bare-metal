/*
 * modbus_port_stm32.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_PORT_STM32_H
#define MODBUS_PORT_STM32_H

#include <stdint.h>

/* RX buffer (extern to main.c) */
extern volatile uint8_t rx_buf[];
extern volatile uint16_t rx_len;
extern volatile uint8_t frame_ready;

/* timestamp */
extern volatile uint32_t msTicks;
extern volatile uint32_t last_rx_ms;

/* error flags */
extern volatile uint8_t neFlag;
extern volatile uint8_t frameFlag;

/* Init */
void Modbus_Port_Init(void);

/* Send byte */
void Modbus_Send_Byte(uint8_t b);

/* Timer reset */
void Modbus_T35_Reset(void);

#endif /* MODBUS_PORT_STM32_H_ */
