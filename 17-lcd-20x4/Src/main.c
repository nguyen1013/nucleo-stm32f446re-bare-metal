#include "main.h"

char line1[] = "Hello LCD 2004";
char line2[] = "Value:";
int value = 123;

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim6_init();

	I2C_Config();
	LCD2004_Init();

	LCD2004_SetCursor(0, 0);
	LCD2004_SendString(line1);

	LCD2004_SetCursor(0, 1);
	LCD2004_SendString(line2);

	LCD2004_PrintIntAt(7, 1, value);

	LCD2004_SetCursor(0, 2);
	LCD2004_SendString("Line 3 here...");

	LCD2004_SetCursor(0, 3);
	LCD2004_SendString("Line 4 here...:12345");

	while (1) {
	}

}
