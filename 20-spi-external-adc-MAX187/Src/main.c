#include "main.h"
#include <stdio.h>

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();

	Uart2_init();
	Tim2_init();
	Tim6_init();

	SPI1_GPIOConfig();
	SPI1_Config();
	SPI1_Enable();
	MAX187_Init();

	delay_ms_tim6(100);
	char buf[100];

	while (1) {

		uint16_t value = MAX187_Read();
		uint16_t voltage = MAX187_ReadVoltage_mV();

		snprintf(buf, sizeof(buf), "ADC value: %d - ", value);
		UART2_SendString(buf);

		snprintf(buf, sizeof(buf), "Voltage: %d mV\r\n", voltage);
		UART2_SendString(buf);

		delay_ms_tim6(1000);

	}
}

