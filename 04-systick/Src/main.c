#include "main.h"

int flag = 0;
uint8_t count = 0;

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	Uart2_init();

	RCC->AHB1ENR |= 1; //GPIOA ABH bus clock ON
	GPIOA->MODER |= 1 << 10; //GPIOA pin 5 to output

	char buf[40];
	sprintf(buf, "Hello world!...\n\r");
	int count = 0;
	UART2_SendString(buf);

	while (1) {
		GPIOA->ODR ^= (1 << 5);

		count++;
		sprintf(buf, "Count = %d\n\r", count);

		UART2_SendString(buf);

		systick_delay_s(1);
	}

}

