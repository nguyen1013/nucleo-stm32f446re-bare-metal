#include "main.h"

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Uart2_init();
	ADC_Init();
	ADC_Enable();

	RCC->AHB1ENR |= 1; //GPIOA ABH bus clock ON
	GPIOA->MODER |= 1 << 10; //GPIOA pin 5 to output

	UART2_SendString("Hello world!...\n\r");

	uint8_t count= 1;

	char buf[100];

	uint16_t ADC_VAL[3] = { 0, 0, 0 };

	while (1) {

		sprintf(buf, "\n\rcount = %d\n\r", count);
		UART2_SendString(buf);
		count++;
		if (count == 100) count = 1;

		ADC_Start(0);
		ADC_WaitForConv();
		ADC_VAL[0] = ADC_GetVal();

		ADC_Start(1);
		ADC_WaitForConv();
		ADC_VAL[1] = ADC_GetVal();

		ADC_Start(4);
		ADC_WaitForConv();
		ADC_VAL[2] = ADC_GetVal();

		sprintf(buf, "VAL_0 = %d, VAL_1 = %d, VAL_2 =  %d\n\r", ADC_VAL[0], ADC_VAL[1], ADC_VAL[2]);
		UART2_SendString(buf);

		delay_ms(1000);
	}
}

