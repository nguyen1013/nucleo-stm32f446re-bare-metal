/*
 * retarget_printf.c
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#include "retarget_printf.h"

int _write(int file, char *ptr, int len) {
	for (int i = 0; i < len; i++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}
