#include "main.h"

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim5_init();

	char buf[40];
	int count = 0;

	while (1) {

		sprintf(buf, "count = %d\n\r", count);
		UART2_SendString(buf);

		count++;
		if (count >= 100)
			count = 0;

		delay_us_tim5(1000000);
	}
}

