/*
 * debug_uart.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Send debug string max maxchars, add \r\n
 */
void write_debug_msg(char *str, int maxchars);

/**
 * Print frame: 0x01,0x04,0x00,...
 */
void write_debug_frame(uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_UART_H */

#endif /* DEBUG_UART_H_ */
