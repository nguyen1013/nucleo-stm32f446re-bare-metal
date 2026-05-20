/*
 * debug_uart.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "debug_uart.h"
#include "uart2.h"   // hoặc file chứa UART2_SendString/UART2_SendChar/Uart2_init

// Bảng đổi nibble sang hex char
static const char bytestr[] = {
    '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
};

void DebugUART_Init(void)
{
    // Nếu bạn muốn gom init UART2 vào đây:
    // Uart2_init();
}

void DebugUART_SendString(const char *s)
{
    // Tận dụng hàm bạn đang có
    UART2_SendString((char*)s);
}

void DebugUART_SendChar(char c)
{
    UART2_SendChar(c);
}

/**
 * Debug write a string to debug terminal
 */
void write_debug_msg(char *str, int maxchars)
{
    int i = 0;
    while (str[i] != '\0')
    {
        UART2_SendChar(str[i]);
        if (++i == maxchars) break;
    }
    UART2_SendChar('\r');
    UART2_SendChar('\n');
}

/**
 * Write Modbus frame bytes to debug terminal
 */
void write_debug_frame(uint8_t *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        UART2_SendChar('0');
        UART2_SendChar('x');
        UART2_SendChar(bytestr[(buf[i] & 0xF0) >> 4]);
        UART2_SendChar(bytestr[buf[i] & 0x0F]);
        UART2_SendChar(',');
    }
    UART2_SendChar('\r');
    UART2_SendChar('\n');
}
