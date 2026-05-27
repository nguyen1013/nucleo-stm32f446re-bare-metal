#include "main.h"
#include <stdio.h>

char tx_buf[64];

int main(void) {
	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();

	Uart2_init();
	Tim6_init();
	Tim7_init();

	ENC_Init();

	enc_state_t cur;
	enc_state_t last = { 0 };

	while (1) {
		ENC_Update(); // polling detect rotation
		cur = ENC_GetState();

		if ((cur.count != last.count) || (cur.btn == BTN_CLICKED)) {
			const char *dir_str = (cur.dir == ENC_DIR_CW) ? "CW" :
									(cur.dir == ENC_DIR_CCW) ? "CCW" : "NONE";

			const char *btn_str =
					(cur.btn == BTN_CLICKED) ? "CLICK" :
					(cur.btn == BTN_PRESSED) ? "PRESSED" : "RELEASED";

			sprintf(tx_buf, "cnt=%ld dir=%s btn=%s\r\n", cur.count, dir_str,
					btn_str);

			UART2_SendString(tx_buf);
		}

		last = cur;
	}
}

/* For encoder button */
void EXTI9_5_IRQHandler(void) {
	ENC_BTN_EXTI_IRQHandler();
}

