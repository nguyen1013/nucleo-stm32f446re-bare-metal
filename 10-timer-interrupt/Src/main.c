#include "main.h"

int count = 0;

int main(void) {
	__disable_irq();

   // Keep connection stlink debug during sleeping
	DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim3_init();

	// Enable clock GPIOA
	RCC->AHB1ENR |= 1;

	// PA5 output
	GPIOA->MODER |= (1 << 10);

    __enable_irq();

	while (1) {
		__WFI();
	}
}

// Interrupt handler TIM3
void TIM3_IRQHandler(void) {
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~TIM_SR_UIF;   // clear flag

		GPIOA->ODR ^= (1 << 5);    // toggle LED

		char buf[40];
		count++;
		sprintf(buf, "counting = %d\n\r", count);
		UART2_SendString(buf);
	}
}

