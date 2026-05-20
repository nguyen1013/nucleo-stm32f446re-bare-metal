/*
 * uart1_interrupt.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "uart1_interrupt.h"

/**
 * Initialize Modbus pins for UART1
 */
void USART1_Init(void) {
//     Enable clocks
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

//     PA9 TX, PA10 RX

//     Clear mode
	GPIOA->MODER &= ~((3 << (9 * 2)) | (3 << (10 * 2)));

//     Alternate function mode
	GPIOA->MODER |= ((2 << (9 * 2)) | (2 << (10 * 2)));

//     High speed
	GPIOA->OSPEEDR |= ((3 << (9 * 2)) | (3 << (10 * 2)));

//     AF7
	GPIOA->AFR[1] &= ~((0xF << 4) | (0xF << 8));

	GPIOA->AFR[1] |= ((7 << 4) | (7 << 8));

//     Optional pullup RX
	GPIOA->PUPDR &= ~((3 << (9 * 2)) | (3 << (10 * 2)));
	GPIOA->PUPDR |= (1 << (10 * 2));

//     Disable USART before config
	USART1->CR1 = 0;

//      APB2 = 90MHz   Baud = 9600
//    USART1->BRR = 0x249F;
	USART1->BRR = 0x30D; // Baud = 115200

//     Enable RX
	USART1->CR1 |= USART_CR1_RE;

//     Enable TX
	USART1->CR1 |= USART_CR1_TE;

//     RX interrupt enable
	USART1->CR1 |= USART_CR1_RXNEIE;

//     USART enable
	USART1->CR1 |= USART_CR1_UE;

//     NVIC
	NVIC_SetPriority(USART1_IRQn, 5);
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_write(char data) {
	//wait while TX buffer is empty
	while (!(USART1->SR & 0x0080)) {
	} 	//TXE: Transmit data register empty. p736-737
	USART1->DR = (data);			//p739
}

char USART1_read() {
	char data = 0;
	//wait while RX buffer is data is ready to be read
	while (!(USART1->SR & 0x0020)) {
	} 	//Bit 5 RXNE: Read data register not empty
	data = USART1->DR;			//p739
	return data;
}
