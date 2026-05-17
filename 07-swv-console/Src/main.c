#include "main.h"

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Tim7_init();
	Uart2_init();
	ADC_Init();
	ADC_Enable();
	DAC_Init_PA4();

	char buf[40];
	int count = 0;

	while (1) {

		sprintf(buf, "count = %d\n\r", count);
		UART2_SendString(buf);
		printf(buf);

		count++;
		if (count >= 100)
			count = 0;

		delay_s(1);
	}
}

