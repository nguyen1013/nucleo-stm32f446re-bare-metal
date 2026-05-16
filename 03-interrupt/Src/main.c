#include "main.h"

int flag = 0;
uint8_t count = 0;

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Tim6_init();
	Uart2_init();

	GPIO_Config();
	Interrupt_Config();

	while (1) {
		__WFI();   // Wait For Interrupt
	}
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (1 << 13))    // If the PC13 triggered the interrupt
			{
		char buf[100];

		flag = 1;
		count++;
		sprintf(buf, "count = %d\n\r", count);
		UART2_SendString(buf);
		flag = 0;
		EXTI->PR |= (1 << 13);  // Clear the interrupt flag by writing a 1
	}
}

