/*
 * spi.h
 *
 *  Created on: 19.5.2026
 *      Author: nn
 */

#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===================== CS (NSS) Pin ===================== */
/* Use PB6 manual CS for adc_max187 */
#define SPI1_CS_GPIO          GPIOB
#define SPI1_CS_PIN           6U

/* ===================== API ===================== */
void SPI1_GPIOConfig(void);
void SPI1_Config(void);

void SPI1_Enable(void);
void SPI1_Disable(void);

void SPI1_CS_Enable(void);
void SPI1_CS_Disable(void);

void SPI1_Transmit(uint8_t *data, int size);
void SPI1_Receive(uint8_t *data, int size);

void SPI1_TransmitReceive(uint8_t *txData, uint8_t *rxData, int size);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H */
