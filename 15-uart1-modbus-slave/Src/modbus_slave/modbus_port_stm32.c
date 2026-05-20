/*
 * modbus_port_stm32.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "modbus_port_stm32.h"
#include "modbus_config.h"
#include "uart1_interrupt.h"
#include "tim4.h"

/* ===== GLOBAL ===== */
volatile uint8_t rx_buf[RX_MAX];
volatile uint16_t rx_len = 0;
volatile uint8_t frame_ready = 0;

volatile uint32_t msTicks = 0;
volatile uint32_t last_rx_ms = 0;

volatile uint8_t neFlag = 0;
volatile uint8_t frameFlag = 0;

/* ===== INIT ===== */
void Modbus_Port_Init(void) {
	USART1_Init();
	TIM4_init_T35();

	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

/* ===== UART SEND ===== */
void Modbus_Send_Byte(uint8_t b) {
	USART1_write((char) b);
}

/* ===== TIMER RESET ===== */
void Modbus_T35_Reset(void) {
	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM4->CNT = 0;
	TIM4->SR &= ~TIM_SR_UIF;
	TIM4->CR1 |= TIM_CR1_CEN;
}

/* ===== TIM4 IRQ ===== */
void TIM4_IRQHandler(void) {
	if (TIM4->SR & TIM_SR_UIF) {
		TIM4->SR &= ~TIM_SR_UIF;
		TIM4->CR1 &= ~TIM_CR1_CEN;

		if (rx_len > 0)
			frame_ready = 1;
	}
}

/* ===== USART IRQ ===== */
void USART1_IRQHandler(void) {
	uint32_t sr = USART1->SR;

	/* Error */
	if (sr & USART_SR_FE) {
		frameFlag = 1;
	}

	if (sr & USART_SR_NE) {
		neFlag = 1;
	}

	if (sr & (USART_SR_FE | USART_SR_NE | USART_SR_ORE)) {
		(void) USART1->DR;
		rx_len = 0;

		TIM4->CR1 &= ~TIM_CR1_CEN;
		TIM4->SR &= ~TIM_SR_UIF;
		return;
	}

	/* RX */
	if (sr & USART_SR_RXNE) {
		uint8_t b = USART1->DR;

		if (rx_len < RX_MAX)
			rx_buf[rx_len++] = b;
		else
			rx_len = 0;

		last_rx_ms = msTicks;
		Modbus_T35_Reset();
	}
}
