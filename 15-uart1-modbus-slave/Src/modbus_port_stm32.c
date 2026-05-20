/*
 * modbus_port_stm32.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "modbus_port_stm32.h"
#include "debug_uart.h"
#include "stm32f4xx.h"

/* extern từ code bạn đang có */
extern volatile uint32_t msTicks;
extern void USART1_write(char c);

extern void UART2_SendString(char *str);
extern void UART2_SendChar(char c);

/* ===== millis ===== */
uint32_t mb_port_millis(void) {
  return (uint32_t)msTicks;
}

/* ===== critical section ===== */
void mb_port_enter_crit(void) { __disable_irq(); }
void mb_port_exit_crit(void)  { __enable_irq();  }

/* ===== send bytes over Modbus UART ===== */
void mb_port_send_bytes(const uint8_t *data, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    USART1_write((char)data[i]);
  }
}

/* ===== debug ===== */
void mb_port_dbg_puts(const char *s) {
#if MB_DEBUG
  /* UART2_SendString expects char* */
  UART2_SendString((char*)s);
#else
  (void)s;
#endif
}

void mb_port_dbg_hexdump(const uint8_t *buf, uint16_t len) {
#if MB_DEBUG
  debug_hexdump(buf, len);
#else
  (void)buf; (void)len;
#endif
}

/* ===== TIM4 T3.5 reset =====
   Lưu ý: Tim4_init() của bạn phải cấu hình ARR sao cho đúng T3.5 theo baud.
*/
void mb_port_t35_reset(void) {
  TIM4->CNT = 0;
  TIM4->SR  &= ~TIM_SR_UIF;
  TIM4->CR1 |= TIM_CR1_CEN;
}
