/*
 * uart2.h
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#ifndef UART2_H
#define UART2_H

#include "stm32f4xx.h"

/* Function Prototypes */
void Uart2_init(void);
void UART2_SendChar(uint8_t c);
void UART2_SendString(char *string);
uint8_t UART2_GetChar(void);

#endif /* UART2_H */
