/*
 * lcd_2004.c
 *
 *  Created on: 25.5.2026
 *  Author: nn
 */

#include "lcd_2004.h"
#include <stdio.h>
#include "tim6.h"

extern int I2C1_Write_LCD(uint8_t address, int n, uint8_t *data);

/*
 * PCF8574 mapping:
 * BL=0x08, EN=0x04, RW=0x02, RS=0x01
 */
#define LCD_BL   0x08
#define LCD_EN   0x04
#define LCD_RW   0x02
#define LCD_RS   0x01

// DDRAM start address for LCD 20x4 (HD44780 mapping)
static const uint8_t row_addr[LCD2004_ROWS] = { 0x00, 0x40, 0x14, 0x54 };

// --- Internal low-level write: send 8-bit value in 4-bit mode via PCF8574 ---
static void lcd2004_sendOneByte(uint8_t nibble_l, uint8_t nibble_r, uint8_t rs) {
	uint8_t data[4];

	// EN high then low for upper nibble
	data[0] = nibble_l | LCD_BL | LCD_EN | (rs ? LCD_RS : 0);
	data[1] = nibble_l | LCD_BL | (rs ? LCD_RS : 0);

	// EN high then low for lower nibble
	data[2] = nibble_r | LCD_BL | LCD_EN | (rs ? LCD_RS : 0);
	data[3] = nibble_r | LCD_BL | (rs ? LCD_RS : 0);

	I2C1_Write_LCD(LCD2004_I2C_ADDR, 4, data);
}

// Send raw 4-bit nibble
static void lcd2004_sendNibble(uint8_t nibble, uint8_t rs) {
	uint8_t data[2];
	uint8_t nibble_l = (uint8_t) (nibble & 0xF0);

	data[0] = nibble_l | LCD_BL | LCD_EN | (rs ? LCD_RS : 0);
	data[1] = nibble_l | LCD_BL | (rs ? LCD_RS : 0);

	I2C1_Write_LCD(LCD2004_I2C_ADDR, 2, data);
}

void LCD2004_Init(void) {
	// Ensure TIM6 ready (safe init once)
	static uint8_t tim6_ready = 0;
	if (!tim6_ready) {
		Tim6_init();
		tim6_ready = 1;
	}

	// Power-up wait: HD44780 needs >15ms
	delay_ms_tim6(20);

	// 4-bit init sequence (recommend for starting lcd into 4-bit mode from 8-bit mode)
	// Send 0x3 three times (as nibble) then 0x2 (set 4-bit mode)
	lcd2004_sendNibble(0x30, 0); //Function set (8-bit mode) x 3 times for stabilization
	delay_ms_tim6(5);
	lcd2004_sendNibble(0x30, 0);
	delay_ms_tim6(1);
	lcd2004_sendNibble(0x30, 0);
	delay_ms_tim6(1);
	lcd2004_sendNibble(0x20, 0); // 4-bit mode
	delay_ms_tim6(1);

	// Function set: 4-bit, 2-line (HD44780 uses 2-line mode even for 4 lines), 5x8 dots
	LCD2004_SendCommand(0x28);

	// Display off (optional)
	LCD2004_SendCommand(0x08);

	// Clear display
	LCD2004_SendCommand(0x01);  // includes 2ms delay inside

	// Entry mode set: increment, no shift
	LCD2004_SendCommand(0x06);

	// Display on, cursor off, blink off
	LCD2004_SendCommand(0x0C);

	// Home
	LCD2004_SendCommand(0x02);  // includes 2ms delay inside
}

void LCD2004_SendCommand(uint8_t cmd) {
	uint8_t nibble_l = cmd & 0xF0;
	uint8_t nibble_r = (cmd << 4) & 0xF0;
	lcd2004_sendOneByte(nibble_l, nibble_r, 0);

	// Datasheet timing: clear/return home need longer delay
	if (cmd <= 0x03) {
		delay_ms_tim6(2);
	} else {
		// small settle (optional, helps with some backpacks)
		// delay_ms_tim6(1);
	}
}

void LCD2004_SendChar(uint8_t c) {
	uint8_t nibble_l = c & 0xF0;
	uint8_t nibble_r = (uint8_t) ((c << 4) & 0xF0);
	lcd2004_sendOneByte(nibble_l, nibble_r, 1);
}

void LCD2004_SendString(const char *str) {
	if (!str)
		return;
	while (*str) {
		LCD2004_SendChar((uint8_t) (*str++));
	}
}

void LCD2004_SetCursor(uint8_t col, uint8_t row) {
	if (row >= LCD2004_ROWS)
		row = 0;
	if (col >= LCD2004_COLS)
		col = 0;

	uint8_t addr = (uint8_t) (row_addr[row] + col);
	LCD2004_SendCommand((uint8_t) (0x80 | addr));
}

void LCD2004_ClearAll(void) {
	LCD2004_SendCommand(0x01); // delay handled inside
}

void LCD2004_ClearRow(uint8_t row) {
	if (row >= LCD2004_ROWS)
		return;

	LCD2004_SetCursor(0, row);
	for (uint8_t i = 0; i < LCD2004_COLS; i++) {
		LCD2004_SendChar(' ');
	}
	LCD2004_SetCursor(0, row);
}

void LCD2004_ClearFromPos(uint8_t col, uint8_t row) {
	if (row >= LCD2004_ROWS)
		return;
	if (col >= LCD2004_COLS)
		return;

	LCD2004_SetCursor(col, row);
	for (uint8_t i = col; i < LCD2004_COLS; i++) {
		LCD2004_SendChar(' ');
	}
	LCD2004_SetCursor(col, row);
}

void LCD2004_PrintIntAt(uint8_t col, uint8_t row, int value) {
	char buf[16];
	snprintf(buf, sizeof(buf), "%d", value);
	LCD2004_SetCursor(col, row);
	LCD2004_SendString(buf);
}

void LCD2004_PrintStringScroll(uint8_t col, uint8_t row, const char *str) {
	if (!str)
		return;

	uint16_t len = strlen(str);

	// If len <= 20, print normally
	if (len <= LCD2004_COLS) {
		LCD2004_SetCursor(col, row);
		LCD2004_SendString(str);

		// filling empty space remaining
		for (uint8_t i = len; i < LCD2004_COLS; i++) {
			LCD2004_SendChar(' ');
		}
		return;
	}

	// --- create buffer extended for smooth scrolling ---
	// adding space before and after
	char extended[128];

	uint8_t padding = 5; // before/after

	uint16_t ext_len = 0;

	// padding before
	for (uint8_t i = 0; i < padding; i++)
		extended[ext_len++] = ' ';

	// main string
	strcpy(&extended[ext_len], str);
	ext_len += len;

	// padding after
	for (uint8_t i = 0; i < padding; i++)
		extended[ext_len++] = ' ';

	extended[ext_len] = '\0';

	// --- LOOP SCROLL ---
	while (1) {
		for (uint16_t i = 0; i <= ext_len - LCD2004_COLS; i++) {
			LCD2004_SetCursor(col, row);

			// print current 20 chars
			for (uint8_t j = 0; j < LCD2004_COLS; j++) {
				LCD2004_SendChar(extended[i + j]);
			}

			delay_ms_tim6(500);   // scrolling speed
		}
	}
}

