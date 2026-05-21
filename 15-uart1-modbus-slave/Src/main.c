#include "main.h"

/* ===== Application callback =====
 * Modbus_slave call this function
 */
int MB_App_ReadInputRegister(uint16_t addr, uint16_t *outValue) {
	(void) addr;
	*outValue = 50;   // simulate sensor value
	return 0;
}

/* ===== MAIN ===== */
int main(void) {
	__disable_irq();

	SystemClock_Config();
	SystemCoreClockUpdate();

	/* Init Modbus port (UART + TIM4) */
	Modbus_Port_Init();

	/* Debug UART */
	Uart2_init();
	UART2_SendString("Hello....\r\n");

	__enable_irq();

	while (1) {

		/* ===== Error from ISR ===== */
		if (frameFlag) {
			UART2_SendString("Framing Error\r\n");
			frameFlag = 0;
		}

		if (neFlag) {
			UART2_SendString("Noise Error\r\n");
			neFlag = 0;
		}

		/* ===== Process frame ===== */
		if (frame_ready) {
			uint16_t len = rx_len;

			/* reset to receive new frame */
			rx_len = 0;
			frame_ready = 0;

			/* debug frame */
			UART2_SendString("\n\rReceived frame:");
			write_debug_frame((uint8_t*) rx_buf, len);

			/* ===== CALL MODBUS LAYER ===== */
			MB_Slave_ProcessFrame((const uint8_t*) rx_buf, len);
		}
	}

	return 0;
}
