#include "main.h"

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Uart2_init();
	ADC_Init();
	ADC_Enable();
	DAC_Init_PA4();

	char buf[40];
	int data = 0;
	uint16_t ADC_VAL = 0;

	while (1) {
		DAC_Write_PA4(data);
		systick_delay_ms(200);

		ADC_Start(0);
		ADC_WaitForConv();
		ADC_VAL = ADC_GetVal();

		sprintf(buf, "DAC = %d\n\r", data);
		UART2_SendString(buf);

		sprintf(buf, "ADC = %d\n\r", ADC_VAL);
		UART2_SendString(buf);

		data += 10;

		if (data >= 4095)
			data = 0;

	}

}

