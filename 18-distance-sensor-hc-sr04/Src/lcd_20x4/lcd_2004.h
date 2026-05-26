/*
 * lcd_20x4.h
 *
 *  Created on: 25.5.2026
 *      Author: nn
 */

#ifndef LCD_2004_H
#define LCD_2004_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// I2C address of module PCF8574 (7-bit)
#ifndef LCD2004_I2C_ADDR
#define LCD2004_I2C_ADDR   0x27
#endif

#define LCD2004_COLS  20
#define LCD2004_ROWS  4

// API
void LCD2004_Init(void);
void LCD2004_SendCommand(uint8_t cmd);
void LCD2004_SendChar(uint8_t c);
void LCD2004_SendString(const char *str);

void LCD2004_SetCursor(uint8_t col, uint8_t row);  // col: 0..19, row: 0..3
void LCD2004_ClearAll(void);
void LCD2004_ClearRow(uint8_t row);
void LCD2004_ClearFromPos(uint8_t col, uint8_t row); // clear from (col,row) to end of line

// Display utilities
void LCD2004_PrintIntAt(uint8_t col, uint8_t row, int value);
void LCD2004_PrintStringScroll(uint8_t col, uint8_t row, const char *str);

#ifdef __cplusplus
}
#endif

#endif // LCD_2004_H
