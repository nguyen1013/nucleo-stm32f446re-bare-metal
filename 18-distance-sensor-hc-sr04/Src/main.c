#include "main.h"

#define NO_DETECTED_OBJECT_ERR 0

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim5_init();
	Tim6_init();

	UART2_SendString("Hello...\n\r");

	HCSR04_TIM3_Init(); // TIM3_CH2 PB5 Input Capture
	char msg[64];

	while (1) {
		uint32_t cm = HCSR04_TIM3_ReadDistance_cm(60000);

		if (cm == NO_DETECTED_OBJECT_ERR) {
			UART2_SendString("No object detected...\n\r");
		} else {
			snprintf(msg, sizeof(msg), "Distance: %lu cm\r\n", cm);
			UART2_SendString(msg);
		}

		delay_ms_tim6(1000);
	}

}
