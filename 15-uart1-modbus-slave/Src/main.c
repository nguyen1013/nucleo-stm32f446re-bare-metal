/* Includes */
#include "main.h"
#include "modbus_crc.h"
#include "uart1_interrupt.h"
#include "tim4.h"

#define SLAVE_ADDRESS 0x01
#define INPUT_REGISTER 0x01

#define MB_EX_ILLEGAL_FUNCTION     0x01
#define MB_EX_ILLEGAL_DATA_ADDR    0x02
#define MB_EX_ILLEGAL_DATA_VALUE   0x03
#define MB_EX_SLAVE_DEVICE_FAILURE 0x04

#define RX_MAX                 256     // RTU max frame 256 bytes

/* Timeout debug */
#define INTERCHAR_TIMEOUT_MS   5       // > 3.5Tchar @9600 (~4ms), @11520 (~2ms)
#define FRAME_STUCK_TIMEOUT_MS 50      // reset and log if not receiving more bytes when >50ms

volatile uint8_t rx_buf[RX_MAX];
volatile uint16_t rx_len = 0;
volatile uint8_t frame_ready = 0;

/* debug counters */
volatile uint32_t dbg_crc_fail = 0;
volatile uint32_t dbg_exc_sent = 0;
volatile uint32_t dbg_frame_stuck = 0;

/* timestamp (ms) */
volatile uint32_t msTicks = 0;
volatile uint32_t last_rx_ms = 0;
volatile uint32_t last_frame_ms = 0;

int read_sensor(int input_address);
void respond_frame(int sensor_value);
void write_debug_msg(char *str, int maxchars);
void write_debug_frame(uint8_t *buf, int len);

volatile uint8_t neFlag = 0;
volatile uint8_t frameFlag = 0;

void modbus_send_exception(uint8_t req_func, uint8_t ex_code) {
	uint8_t tx[5];
	tx[0] = SLAVE_ADDRESS;
	tx[1] = (uint8_t) (req_func | 0x80); // set MSB = exception
	tx[2] = ex_code;

	uint16_t crc = CRC16((char*) tx, 3);
	tx[3] = (uint8_t) (crc & 0xFF); // CRC low first
	tx[4] = (uint8_t) ((crc >> 8) & 0xFF);

	for (int i = 0; i < 5; i++) {
		USART1_write((char) tx[i]);
	}
	dbg_exc_sent++;
}

static inline uint32_t millis(void) {
	return msTicks;
}

static inline void T35_Timer_Reset(void) {
	TIM4->CNT = 0;               // reset counter
	TIM4->SR &= ~TIM_SR_UIF;     // clear flag
	TIM4->CR1 |= TIM_CR1_CEN;    // start timer
}

int main(void) {
	__disable_irq();

	/* Configure the system clock to 180 MHz and update SystemCoreClock */
	SystemClock_Config();
	SystemCoreClockUpdate();

	USART1_Init(); // ModBus
	Uart2_init(); // Used as debugging terminal
	Tim6_init();
	Tim4_init();

	UART2_SendString("Hello....\n\r");

	USART1->CR1 |= 0x0020;			//enable RX interrupt
	NVIC_EnableIRQ(USART1_IRQn); 	//enable interrupt in NVIC
	__enable_irq();				//global enable IRQs

	while (1) {
		/* ===== Error flags from ISR ===== */
		if (frameFlag) {
			UART2_SendString("Framing Error Detected\r\n");
			frameFlag = 0;
		}
		if (neFlag) {
			UART2_SendString("Noise Error Detected\r\n");
			neFlag = 0;
		}

		/* ===== Timeout debug: stuck partial frame ===== */
		uint32_t now = millis();

		if (!frame_ready && rx_len > 0) {
			/* wait for new bytes, if not receive new bytes -> reset buffer */
			if ((now - last_rx_ms) > FRAME_STUCK_TIMEOUT_MS) {
				UART2_SendString("RX stuck timeout -> drop partial frame\r\n");
				dbg_frame_stuck++;
				__disable_irq();
				rx_len = 0;
				__enable_irq();
			}
		}

		/* ===== Process complete frame ===== */
		if (frame_ready) {
			__disable_irq();
			uint16_t len = rx_len;
			frame_ready = 0;
			rx_len = 0;            // reset to receive new frame
			__enable_irq();

			last_frame_ms = now;

			UART2_SendString("received frame:\r\n");
			write_debug_frame((uint8_t*) rx_buf, len);

			/* 1) Basic length check */
			if (len < 4) {
				UART2_SendString("Frame too short\r\n");
				continue;
			}

			/* 2) Address check */
			if (rx_buf[0] != SLAVE_ADDRESS) {
				UART2_SendString("Not my address -> ignore\r\n");
				continue;
			}

			/* 3) CRC check (CRC is last 2 bytes, low then high) */
			if (len < 5) { // address+func+ex/data + crc(2)
				UART2_SendString("Frame too short for CRC\r\n");
				continue;
			}

			uint16_t calc_crc = CRC16((char*) rx_buf, (int) (len - 2));
			uint8_t crc_lo = (uint8_t) (calc_crc & 0xFF);
			uint8_t crc_hi = (uint8_t) ((calc_crc >> 8) & 0xFF);

			if (rx_buf[len - 2] != crc_lo || rx_buf[len - 1] != crc_hi) {
				UART2_SendString("CRC Fail (drop)\r\n");
				dbg_crc_fail++;
				/* Modbus RTU: wrong CRC, master will be timeout */
				continue;
			}

			UART2_SendString("CRC OK\r\n");

			/* 4) Parse function */
			uint8_t func = rx_buf[1];

			/* Example: Support FC=0x04 Read Input Registers only */
			if (func == 0x04) {
				/* Request RTU for FC04 with len = 8:
				 [addr][04][startHi][startLo][qtyHi][qtyLo][crclo][crchi]
				 */
				if (len != 8) {
					UART2_SendString("FC04 invalid length\r\n");
					modbus_send_exception(func, MB_EX_ILLEGAL_DATA_VALUE); // wrong frame/length
					continue;
				}

				uint16_t start_addr = ((uint16_t) rx_buf[2] << 8) | rx_buf[3];
				uint16_t quantity = ((uint16_t) rx_buf[4] << 8) | rx_buf[5];

				if (quantity != 1) {
					UART2_SendString("FC04 qty != 1\r\n");
					modbus_send_exception(func, MB_EX_ILLEGAL_DATA_VALUE); // value/quantity incorrect
					continue;
				}

				if (start_addr != INPUT_REGISTER) {
					UART2_SendString("FC04 wrong register\r\n");
					modbus_send_exception(func, MB_EX_ILLEGAL_DATA_ADDR); // address out of range
					continue;
				}

				int sensor_value = read_sensor(INPUT_REGISTER);
				respond_frame(sensor_value);
			} else {
				UART2_SendString("Unsupported function -> exception 01\r\n");
				modbus_send_exception(func, MB_EX_ILLEGAL_FUNCTION); // illegal function
			}
		}
	}

	return 0;
}

void TIM4_IRQHandler(void) {
	if (TIM4->SR & TIM_SR_UIF) {
		TIM4->SR &= ~TIM_SR_UIF;
		TIM4->CR1 &= ~TIM_CR1_CEN;

		if (rx_len > 0)
			frame_ready = 1;
	}
}

void USART1_IRQHandler(void) {
	uint32_t sr = USART1->SR;

	// ---- Error handling ----
	if (sr & (USART_SR_FE | USART_SR_NE | USART_SR_ORE)) {
		volatile uint32_t tmp = USART1->DR;
		(void) tmp;
		rx_len = 0;
		return;
	}

// RX data
	if (sr & USART_SR_RXNE) {
		uint8_t b = USART1->DR;

		if (rx_len < RX_MAX)
			rx_buf[rx_len++] = b;
		else
			rx_len = 0;

		T35_Timer_Reset();
	}

// IDLE line => end of frame
	if (sr & USART_SR_IDLE) {
		volatile uint32_t tmp = USART1->DR;
		(void) tmp;  // clear IDLE by SR/DR sequence
		if (rx_len > 0)
			frame_ready = 1;
	}
}

int read_sensor(int input_address) {

	return 50;
}

void respond_frame(int sensor_value) {
	GPIOA->ODR |= 0x20;				//led on, transmitting mode
//example response should be like this: 0104020B057E03
	char tx_buf_frame[7] = { SLAVE_ADDRESS, 0x04, 0x02, 0, 0, 0, 0 };
	char sensor_high_bits = 0;
	char sensor_low_bits = 0;
	char crc_high_byte = 0;
	char crc_low_byte = 0;
	unsigned short int crc = 0; //16 bits

	sensor_high_bits = (sensor_value >> 8) | sensor_high_bits;
	sensor_low_bits = sensor_value | sensor_low_bits;
	tx_buf_frame[3] = sensor_high_bits;
	tx_buf_frame[4] = sensor_low_bits;
	crc = CRC16(tx_buf_frame, 5);
	crc_high_byte = (crc >> 8) | crc_high_byte; //high byte
	crc_low_byte = crc | crc_low_byte; //low byte

	tx_buf_frame[6] = crc_high_byte;
	tx_buf_frame[5] = crc_low_byte;

	for (int i = 0; i < 7; i++) {
		USART1_write(tx_buf_frame[i]);
	}

}

/**
 * Debug write a string to debug terminal
 */
void write_debug_msg(char *str, int maxchars) {
	int i = 0;
	while (str[i] != '\0') {
		UART2_SendChar(str[i]);
		if (++i == maxchars)
			break;
	}
	UART2_SendChar('\r');
	UART2_SendChar('\n');
}

char bytestr[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
		'c', 'd', 'e', 'f' };

/**
 * Write Modbus frame bytes to debug terminal
 */
void write_debug_frame(uint8_t *buf, int len) {
	for (int i = 0; i < len; i++) {
		UART2_SendChar('0');
		UART2_SendChar('x');
		UART2_SendChar(bytestr[(buf[i] & 0xF0) >> 4]);
		UART2_SendChar(bytestr[buf[i] & 0x0F]);
		UART2_SendChar(',');
	}
	UART2_SendChar('\r');
	UART2_SendChar('\n');
}

