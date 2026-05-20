/*
 * uart1_interrupt.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef UART1_INTERRUPT_H_
#define UART1_INTERRUPT_H_

#include "stm32f4xx.h"

void USART1_Init(void);

void USART1_write(char data);

char USART1_read();


#endif /* UART1_INTERRUPT_H_ */
