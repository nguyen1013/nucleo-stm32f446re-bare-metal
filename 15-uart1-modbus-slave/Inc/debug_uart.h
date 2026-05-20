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
 * Khởi tạo debug UART (nếu bạn muốn gom init debug vào đây).
 * Nếu bạn đã init UART2 ở nơi khác thì có thể không cần dùng hàm này.
 */
void DebugUART_Init(void);

/**
 * Gửi chuỗi ra UART debug (UART2).
 */
void DebugUART_SendString(const char *s);

/**
 * Gửi 1 ký tự ra UART debug (UART2).
 */
void DebugUART_SendChar(char c);

/**
 * Gửi chuỗi debug tối đa maxchars, tự thêm \r\n
 */
void write_debug_msg(char *str, int maxchars);

/**
 * In frame theo dạng: 0x01,0x04,0x00,...
 */
void write_debug_frame(uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_UART_H */

#endif /* DEBUG_UART_H_ */
