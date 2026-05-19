#include "main.h"

/* =========================
 * MAIN
 * ========================= */

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();

	Uart2_init();
	Tim6_init();

	InternalTemp_Init();

	char buf[100];

	while (1) {

		float t = InternalTemp_ReadC();
		sprintf(buf, "%.2f\r\n", t);
		UART2_SendString(buf);

		delay_ms_tim6(2000);
	}
}
