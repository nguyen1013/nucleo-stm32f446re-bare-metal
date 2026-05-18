#include "main.h"

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Uart2_init();

	RCC->AHB1ENR |= 1; //GPIOA ABH bus clock ON
	GPIOA->MODER |= 1 << 10; //GPIOA pin 5 to output

	char buf[40];
	sprintf(buf, "Testing uart...\n\r");

	while (1) {
//		  GPIOA->ODR|=0x20; //0010 0000 set bit 5. p186
		GPIOA->BSRR |= 1 << 5;
		delay_ms(1000);
//		  GPIOA->ODR&=~0x20; //0000 0000 clear bit 5. p186
		GPIOA->BSRR |= 1 << 21;

		UART2_SendString(buf);
		UART2_SendString("Hello world!...\n\r");


		delay_ms(1000);

//		UART2_SendChar(UART2_GetChar()); //echo

	}
}

