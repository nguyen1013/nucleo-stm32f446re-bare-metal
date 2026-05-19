/*
 * spi.c
 *
 *  Created on: 19.5.2026
 *      Author: nn
 */

#include "spi.h"

void SPI1_Config(void) {
	/************** STEPS TO FOLLOW *****************
	 1. Enable SPI clock
	 2. Configure the Control Register 1
	 3. Configure the CR2
	 ************************************************/

	RCC->APB2ENR |= (1U << 12);   // Enable SPI1 clock

	/* CR1 config */
	SPI1->CR1 |= (1U << 0) | (1U << 1);   // CPOL=1, CPHA=1

	SPI1->CR1 |= (1U << 2);               // MSTR=1 (Master mode)

	SPI1->CR1 |= (3U << 3);               // BR=011 => fPCLK/16 (80MHz/16=5MHz)

	SPI1->CR1 &= ~(1U << 7);              // LSBFIRST=0 (MSB first)

	SPI1->CR1 |= (1U << 8) | (1U << 9); // SSM=1, SSI=1 (Software NSS management)

	SPI1->CR1 &= ~(1U << 10);             // RXONLY=0 (full duplex)

	SPI1->CR1 &= ~(1U << 11);             // DFF=0 (8-bit data)

	/* CR2 default */
	SPI1->CR2 = 0;
}

void SPI1_GPIOConfig(void) {
	RCC->AHB1ENR |= (1U << 0);  // Enable GPIOA clock

	/* PA5=SCK, PA6=MISO, PA7=MOSI => Alternate Function (10)
	 PA9=CS => Output (01)
	 */

	/* MODER: set PA5,6,7 to AF (10), PA9 to Output (01) */
	GPIOA->MODER &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2))
			| (3U << (9 * 2)));
	GPIOA->MODER |= ((2U << (5 * 2)) | (2U << (6 * 2)) | (2U << (7 * 2))
			| (1U << (9 * 2)));

	/* OSPEEDR: High speed for PA5,6,7,9 (11) */
	GPIOA->OSPEEDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2))
			| (3U << (9 * 2)));
	GPIOA->OSPEEDR |= ((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2))
			| (3U << (9 * 2)));

	/* AFRL: AF5 for PA5, PA6, PA7 */
	GPIOA->AFR[0] &=
			~((0xFU << (5 * 4)) | (0xFU << (6 * 4)) | (0xFU << (7 * 4)));
	GPIOA->AFR[0] |= ((5U << (5 * 4)) | (5U << (6 * 4)) | (5U << (7 * 4)));

	/* PA9 output default HIGH (CS inactive) */
	GPIOA->BSRR = (1U << SPI1_CS_PIN);
}

void SPI1_Enable(void) {
	SPI1->CR1 |= (1U << 6);   // SPE=1
}

void SPI1_Disable(void) {
	SPI1->CR1 &= ~(1U << 6);  // SPE=0
}

void SPI1_CS_Enable(void) {
	/* Reset bit => kéo xuống 0 */
	SPI1_CS_GPIO->BSRR = (1U << SPI1_CS_PIN) << 16;
}

void SPI1_CS_Disable(void) {
	/* Set bit => kéo lên 1 */
	SPI1_CS_GPIO->BSRR = (1U << SPI1_CS_PIN);
}

void SPI1_Transmit(uint8_t *data, int size) {
	int i = 0;
	while (i < size) {
		while (!(SPI1->SR & (1U << 1))) {
		}  // TXE=1?
		SPI1->DR = data[i];
		i++;
	}

	/* Wait TXE then BSY cleared */
	while (!(SPI1->SR & (1U << 1))) {
	}      // TXE=1
	while ((SPI1->SR & (1U << 7))) {
	}      // BSY=0

	/* Clear OVR: read DR then SR */
	(void) SPI1->DR;
	(void) SPI1->SR;
}

void SPI1_Receive(uint8_t *data, int size) {
	while (size) {
		while ((SPI1->SR & (1U << 7))) {
		}  // BSY=0
		SPI1->DR = 0x00;                     // dummy byte to clock data
		while (!(SPI1->SR & (1U << 0))) {
		}  // RXNE=1?
		*data++ = (uint8_t) SPI1->DR;
		size--;
	}
}

void SPI1_TransmitReceive(uint8_t *txData, uint8_t *rxData, int size) {
	while (size) {
		// 1. Đợi TX buffer trống
		while (!(SPI1->SR & (1 << 1)))
			;   // TXE

		// 2. Gửi dữ liệu
		SPI1->DR = *txData++;

		// 3. Đợi có dữ liệu nhận
		while (!(SPI1->SR & (1 << 0)))
			;   // RXNE

		// 4. Đọc dữ liệu
		*rxData++ = (uint8_t) SPI1->DR;

		size--;
	}

	// 5. Đợi transmission complete
	while (SPI1->SR & (1 << 7))
		;   // BSY = 0

	// 6. Clear OVR flag
	(void) SPI1->DR;
	(void) SPI1->SR;
}

