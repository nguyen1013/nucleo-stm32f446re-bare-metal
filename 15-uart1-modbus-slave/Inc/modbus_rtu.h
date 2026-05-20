/*
 * modbus_rtu.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "modbus_config.h"

typedef struct {
  volatile uint8_t  buf[MB_RX_MAX];
  volatile uint16_t len;
  volatile bool     frame_ready;

  volatile uint32_t last_rx_ms;
  volatile uint32_t last_frame_ms;

  /* counters */
  volatile uint32_t crc_fail;
  volatile uint32_t exc_sent;
  volatile uint32_t frame_stuck;

  /* error flags */
  volatile uint8_t fe;
  volatile uint8_t ne;
  volatile uint8_t ore;
} mb_rtu_ctx_t;

/* init context */
void mb_rtu_init(mb_rtu_ctx_t *ctx);

/* called from USART RX ISR when one byte arrives */
void mb_rtu_on_rx_byte(mb_rtu_ctx_t *ctx, uint8_t b, uint32_t now_ms);

/* called from TIM4 IRQ when T3.5 expires */
void mb_rtu_on_t35_expired(mb_rtu_ctx_t *ctx, uint32_t now_ms);

/* called periodically in main loop */
void mb_rtu_poll(mb_rtu_ctx_t *ctx);

#endif /* MODBUS_RTU_H_ */
