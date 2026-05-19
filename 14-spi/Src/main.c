#include "main.h"

uint8_t txData[5] = { 1, 2, 3, 4, 5 };
uint8_t rxData[5];

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();
	Uart2_init();
	Tim6_init();

	SPI1_GPIOConfig();
	SPI1_Config();
	SPI1_Enable();

	delay_ms_tim6(2000);
	UART2_SendString("Starting SPI Loopback Test...\n\r");
	delay_ms_tim6(2000);

	char count = 1;

	while (1) {
		/* Option 1 */
		/*		SPI1_CS_Enable();

		 for (int i = 0; i < 5; i++) {
		 // wait TX empty
		 while (!(SPI1->SR & (1 << 1)))
		 ;

		 SPI1->DR = (txData[i] + count) % 100;

		 // wait receive
		 while (!(SPI1->SR & (1 << 0)))
		 ;

		 rxData[i] = SPI1->DR;
		 }

		 count++;

		 SPI1_CS_Disable();*/

		/* Option 2: Using SPI1_TransmitReceive */
		// Update data TX each loop
		for (int i = 0; i < 5; i++) {
			txData[i] = (txData[i] + count) % 100;
		}

		SPI1_CS_Enable();

		SPI1_TransmitReceive(txData, rxData, 5);

		SPI1_CS_Disable();

		/* Debugging */
		char buffer[50];

		UART2_SendString("TX: ");
		for (int i = 0; i < 5; i++) {
			sprintf(buffer, "%d ", txData[i]);
			UART2_SendString(buffer);
		}

		UART2_SendString("\r\nRX: ");
		for (int i = 0; i < 5; i++) {
			sprintf(buffer, "%d ", rxData[i]);
			UART2_SendString(buffer);
		}

		UART2_SendString("\r\n----------------------\r\n");

		delay_ms_tim6(1000);
	}
}
