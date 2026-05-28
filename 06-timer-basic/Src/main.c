#include "main.h"

extern volatile uint32_t g_tim7_ms;

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Tim7_init();
	Uart2_init();

	char buf[40];
	int count = 0;

	uint32_t last = g_tim7_ms;

	while (1) {

		uint32_t now = g_tim7_ms;

		if (now - last >= 1000) {
			last = now;
			UART2_SendString("A second has been passed... ");
			sprintf(buf, "Count = %d\n\r", count);
			UART2_SendString(buf);
			count++;
		}

	}

}

