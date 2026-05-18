#include "main.h"

int i = 0;
int line_ready = 0;
char buf[40];

int main(void) {
	__disable_irq();

	// Keep connection stlink debug during sleeping
	DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();

	NVIC_EnableIRQ(USART2_IRQn);
	__enable_irq();

	while (1) {
		if (line_ready) {
			line_ready = 0;
			UART2_SendString(buf);
			UART2_SendString("\r\n");
		}
		__WFI();
	}

}

void USART2_IRQHandler(void) {
	if (USART2->SR & (1 << 5)) {          // RXNE
		char c = (char) USART2->DR;        // read DR and clear RXNE

		if (!line_ready) {
			if (i < (int) sizeof(buf) - 1) {
				buf[i++] = c;
			}

			if (c == '\r' || i==38) {              // Enter or max length
				buf[i - 1] = '\0';        // replcae '\r' by end of string '\0'
				line_ready = 1;
				i = 0;
			}
		}
	}
}
