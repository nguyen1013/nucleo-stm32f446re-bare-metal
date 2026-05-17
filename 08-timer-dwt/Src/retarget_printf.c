/*
 * retarget_printf.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "retarget_printf.h"

/* retarget printf to swv console */
int _write(int file, char *ptr, int len) {
	for (int i = 0; i < len; i++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}

/* retarget printf to USART2 */
/*
 int __io_putchar(int ch)
 {
 UART2_SendChar(ch);

 return ch;
 }
 */

