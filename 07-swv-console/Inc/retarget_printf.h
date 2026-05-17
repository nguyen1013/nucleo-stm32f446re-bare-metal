/*
 * retarget_printf.h
 *
 *  Created on: 17.5.2026
 *      Author: nn
 */

#ifndef RETARGET_PRINTF_H_
#define RETARGET_PRINTF_H_

#include "stm32f4xx.h"
#include <stdio.h>

int _write(int file, char *ptr, int len);

#endif /* RETARGET_PRINTF_H_ */
