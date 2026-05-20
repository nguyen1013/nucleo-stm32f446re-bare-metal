/*
 * modbus_rtu.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "modbus_rtu.h"
#include "modbus_port_stm32.h"
#include "app_registers.h"
#include "modbus_crc.h"   /* CRC16(char* buf, int len) */

static bool mb_crc_ok(const uint8_t *frame, uint16_t len) {
  if (len < 4) return false;
  uint16_t calc = CRC16((char*)frame, (int)(len - 2));
  uint8_t lo = (uint8_t)(calc & 0xFF);
  uint8_t hi = (uint8_t)(calc >> 8);
  return (frame[len-2] == lo) && (frame[len-1] == hi);
}

/* Build exception response: [addr][func|0x80][ex][crclo][crchi] */
static uint16_t mb_build_exception(uint8_t addr, uint8_t func, uint8_t ex, uint8_t *out) {
  out[0] = addr;
  out[1] = (uint8_t)(func | 0x80);
  out[2] = ex;

  uint16_t crc = CRC16((char*)out, 3);
  out[3] = (uint8_t)(crc & 0xFF);   /* low first */
  out[4] = (uint8_t)(crc >> 8);
  return 5;
}

/* FC04 response for 1 register: [addr][04][02][hi][lo][crclo][crchi] */
static uint16_t mb_build_fc04_resp(uint8_t addr, uint16_t reg, uint8_t *out) {
  out[0] = addr;
  out[1] = 0x04;
  out[2] = 0x02;
  out[3] = (uint8_t)(reg >> 8);
  out[4] = (uint8_t)(reg & 0xFF);

  uint16_t crc = CRC16((char*)out, 5);
  out[5] = (uint8_t)(crc & 0xFF);   /* low first */
  out[6] = (uint8_t)(crc >> 8);
  return 7;
}

void mb_rtu_init(mb_rtu_ctx_t *ctx) {
  ctx->len = 0;
  ctx->frame_ready = false;
  ctx->last_rx_ms = 0;
  ctx->last_frame_ms = 0;

  ctx->crc_fail = 0;
  ctx->exc_sent = 0;
  ctx->frame_stuck = 0;

  ctx->fe = ctx->ne = ctx->ore = 0;
}

void mb_rtu_on_rx_byte(mb_rtu_ctx_t *ctx, uint8_t b, uint32_t now_ms) {
  if (ctx->len < MB_RX_MAX) {
    ctx->buf[ctx->len++] = b;
  } else {
    /* overflow => drop frame */
    ctx->len = 0;
  }
  ctx->last_rx_ms = now_ms;
  mb_port_t35_reset();
}

void mb_rtu_on_t35_expired(mb_rtu_ctx_t *ctx, uint32_t now_ms) {
  (void)now_ms;
  if (ctx->len > 0) {
    ctx->frame_ready = true;
  }
}

/* dispatch only FC04 qty=1 start==MB_INPUT_REGISTER_ADDR */
static void mb_handle_frame(mb_rtu_ctx_t *ctx, const uint8_t *rx, uint16_t len) {
  /* Basic checks */
  if (len < 4) return;
  if (rx[0] != MB_SLAVE_ADDRESS) return;
  if (!mb_crc_ok(rx, len)) { ctx->crc_fail++; return; }

  uint8_t func = rx[1];
  uint8_t tx[MB_RX_MAX];
  uint16_t txlen = 0;

  if (func == 0x04) {
    /* Request: [addr][04][startHi][startLo][qtyHi][qtyLo][crclo][crchi] => len=8 */
    if (len != 8) {
      txlen = mb_build_exception(MB_SLAVE_ADDRESS, func, MB_EX_ILLEGAL_DATA_VALUE, tx);
      ctx->exc_sent++;
      mb_port_send_bytes(tx, txlen);
      return;
    }

    uint16_t start = (uint16_t)((rx[2] << 8) | rx[3]);
    uint16_t qty   = (uint16_t)((rx[4] << 8) | rx[5]);

    if (qty != 1) {
      txlen = mb_build_exception(MB_SLAVE_ADDRESS, func, MB_EX_ILLEGAL_DATA_VALUE, tx);
      ctx->exc_sent++;
      mb_port_send_bytes(tx, txlen);
      return;
    }

    uint16_t value = 0;
    if (!app_read_input_register(start, &value)) {
      txlen = mb_build_exception(MB_SLAVE_ADDRESS, func, MB_EX_ILLEGAL_DATA_ADDR, tx);
      ctx->exc_sent++;
      mb_port_send_bytes(tx, txlen);
      return;
    }

    txlen = mb_build_fc04_resp(MB_SLAVE_ADDRESS, value, tx);
    mb_port_send_bytes(tx, txlen);
    return;
  }

  /* Unsupported function */
  txlen = mb_build_exception(MB_SLAVE_ADDRESS, func, MB_EX_ILLEGAL_FUNCTION, tx);
  ctx->exc_sent++;
  mb_port_send_bytes(tx, txlen);
}

void mb_rtu_poll(mb_rtu_ctx_t *ctx) {
  uint32_t now = mb_port_millis();

  /* optional: stuck watchdog */
  if (!ctx->frame_ready && ctx->len > 0) {
    if ((now - ctx->last_rx_ms) > MB_FRAME_STUCK_TIMEOUT_MS) {
      ctx->frame_stuck++;
      mb_port_enter_crit();
      ctx->len = 0;
      mb_port_exit_crit();
#if MB_DEBUG
      mb_port_dbg_puts("RX stuck -> drop partial frame\r\n");
#endif
    }
  }

  if (!ctx->frame_ready) return;

  uint8_t frame[MB_RX_MAX];
  uint16_t len;

  mb_port_enter_crit();
  len = ctx->len;
  if (len > MB_RX_MAX) len = 0;
  for (uint16_t i = 0; i < len; i++) frame[i] = ctx->buf[i];
  ctx->len = 0;
  ctx->frame_ready = false;
  mb_port_exit_crit();

  ctx->last_frame_ms = now;

#if MB_DEBUG
  mb_port_dbg_puts("Received frame:");
  mb_port_dbg_hexdump(frame, len);
#endif

  mb_handle_frame(ctx, frame, len);
}

