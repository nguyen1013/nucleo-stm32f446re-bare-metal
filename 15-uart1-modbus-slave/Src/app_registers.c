/*
 * app_registers.c
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#include "app_registers.h"
#include "modbus_config.h"

/* Ví dụ của bạn: read_sensor() trả 50 */
static uint16_t read_sensor_stub(uint16_t addr) {
  (void)addr;
  return 50;
}

bool app_read_input_register(uint16_t addr, uint16_t *out_value) {
  if (addr != MB_INPUT_REGISTER_ADDR) return false;
  *out_value = read_sensor_stub(addr);
  return true;
}

