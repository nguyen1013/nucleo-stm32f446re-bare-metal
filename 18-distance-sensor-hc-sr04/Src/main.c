#include "main.h"

#define NO_DETECTED_OBJECT_ERR 0

char line1[] = "Hello LCD 2004";
char line2[] = "Distance:";
volatile float distance_val = 0.0f;

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim5_init();
	Tim6_init();

	UART2_SendString("Hello...\n\r");
	char msg[64];

	/* Distance sensor */
	HCSR04_TIM3_Init(); // TIM3_CH2 PB5 Input Capture

	/* LCD 2004 */
	I2C_Config();
	LCD2004_Init();

	LCD2004_SetCursor(0, 0);
	LCD2004_SendString(line1);

	LCD2004_SetCursor(0, 1);
	LCD2004_SendString(line2);

	LCD2004_PrintFloatAt(10, 1, distance_val);

	delay_ms_tim6(5000);

	while (1) {
		uint32_t cm = HCSR04_TIM3_ReadDistance_cm(60000);

		LCD2004_ClearFromPos(10, 1);

		if (cm == NO_DETECTED_OBJECT_ERR) {
			UART2_SendString("No object detected.\n\r");
			LCD2004_PrintFloatAt(10, 1, distance_val);
			LCD2004_SetCursor(10, 1);
			LCD2004_SendString("ERROR...");
		} else {
			snprintf(msg, sizeof(msg), "Distance: %lu cm\r\n", cm);
			UART2_SendString(msg);
			LCD2004_PrintIntAt(10, 1, cm);
		}

		delay_ms_tim6(1000);
	}

}
