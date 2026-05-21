#include "main.h"
#include "modbus_slave.h"
#include "modbus_port_stm32.h"
#include "debug_uart.h"

/* ===== Application callbacks ===== */
/* Multi-register version */
int MB_App_ReadInputRegisters(uint16_t addr, uint16_t qty, uint16_t *outBuf) {
	(void) addr;

	for (uint16_t i = 0; i < qty; i++) {
		outBuf[i] = 50 + i;   // simulate multiple sensor values
	}

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
			uint16_t len;

			/* --- Critical section (protect shared variables) --- */
			__disable_irq();
			len = rx_len;
			rx_len = 0;
			frame_ready = 0;
			__enable_irq();

			/* Debug frame (optional) */
			UART2_SendString("\r\nReceived frame:");
			write_debug_frame((uint8_t*) rx_buf, len);

			/* ===== Pass to Modbus layer (dispatch + handler) ===== */
			MB_Slave_ProcessFrame((const uint8_t*) rx_buf, len);
		}
	}

	/* Never reached */
	return 0;
}
