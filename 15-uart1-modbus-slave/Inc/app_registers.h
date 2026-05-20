/*
 * app_registers.h
 *
 *  Created on: 20.5.2026
 *      Author: nn
 */

#ifndef APP_REGISTERS_H_
#define APP_REGISTERS_H_

#pragma once
#include <stdint.h>
#include <stdbool.h>

/* App callback: đọc input register (16-bit) */
bool app_read_input_register(uint16_t addr, uint16_t *out_value);

#endif /* APP_REGISTERS_H_ */
