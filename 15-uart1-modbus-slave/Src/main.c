/* Includes */
#include "main.h"
#include "modbus_crc.h"
#include "uart1_interrupt.h"

#define SLAVE_ADDRESS 0x01
#define INPUT_REGISTER 0x01

#define RX_MAX  64
volatile uint8_t rx_buf[RX_MAX];
volatile uint16_t rx_len = 0;
volatile uint8_t frame_ready = 0;

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

void wrong_slave_address(void);
void read_7_bytes_from_usartx(char *received_frame);
int read_sensor(int input_address);
void respond_frame(int sensor_value);
void write_debug_msg(char *str, int maxchars);
void write_debug_frame(char *str, int maxchars);

/************************************************************************/
/* Flags are volatile, since the flag value can change during interrupt */
/* handler. Therefore compiler can handle it correctly.                 */
/************************************************************************/
volatile char mFlag = 0;
volatile uint8_t neFlag = 0;
volatile uint8_t frameFlag = 0;

/*
 * Clear buffer. This is used only for receiver buffer, hence fixed size (8 bytes)
 */
void clear_buffer(char *b) {
	for (int i = 0; i < 8; i++)
		b[i] = 0;
}

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void) {
	__disable_irq();		//global disable IRQs, M3_Generic_User_Guide p135.

	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SystemClock_Config();
	SystemCoreClockUpdate();

	USART1_Init(); // ModBus
	Uart2_init(); // Used as debugging terminal
	Tim6_init();

	UART2_SendString("Hello....\n\r");

	/* TODO - Add your application code here */

	USART1->CR1 |= 0x0020;			//enable RX interrupt
	NVIC_EnableIRQ(USART1_IRQn); 	//enable interrupt in NVIC
	__enable_irq();				//global enable IRQs, M3_Generic_User_Guide p135

	unsigned short int crc = 0; //16 bitts

	while (1) {
		while (1) {
			/* ===== 1. Handle UART error flags ===== */
			if (frameFlag == 1) {
				UART2_SendString("Framing Error Detected\r\n");
				frameFlag = 0;
			}

			if (neFlag == 1) {
				UART2_SendString("Noise Error Detected\r\n");
				neFlag = 0;
			}

			/* ===== 2. Process received Modbus frame ===== */
			if (frame_ready == 1) {
				__disable_irq();
				uint16_t len = rx_len;
				frame_ready = 0;
				__enable_irq();

				UART2_SendString("received frame....\n\r");
				write_debug_frame((char*) rx_buf, len);

				/* ===== 2.1 Basic validation ===== */
				if (len < 8) {
					UART2_SendString("Frame too short\r\n");
					rx_len = 0;
					continue;
				}

				/* ===== 2.2 Check slave address ===== */
				if (rx_buf[0] != SLAVE_ADDRESS) {
					UART2_SendString("Wrong slave address\r\n");
					rx_len = 0;
					continue;
				}

				/* ===== 2.3 CRC check ===== */
				crc = CRC16((char*) rx_buf, len - 2);

				uint8_t crc_low = (uint8_t) (crc & 0xFF);
				uint8_t crc_high = (uint8_t) ((crc >> 8) & 0xFF);

				/* Modbus: CRC low byte first, then high byte */
				if ((rx_buf[len - 2] != crc_low)
						|| (rx_buf[len - 1] != crc_high)) {
					UART2_SendString("CRC Fail\r\n");
					rx_len = 0;
					continue;
				}

				UART2_SendString("CRC OK\r\n");

				/* ===== 2.4 Parse request ===== */
				uint8_t func = rx_buf[1];

				/* Only support function 0x04 (Read Input Registers) */
				if (func == 0x04) {
					uint16_t start_addr = ((uint16_t) rx_buf[2] << 8)
							| rx_buf[3];
					uint16_t quantity = ((uint16_t) rx_buf[4] << 8) | rx_buf[5];

					/* Debug */
					UART2_SendString("Function 0x04\r\n");

					if (start_addr == INPUT_REGISTER && quantity == 1) {
						int sensor_value = read_sensor(INPUT_REGISTER);

						respond_frame(sensor_value);
					} else {
						UART2_SendString("Invalid register or quantity\r\n");
						/* Có thể thêm exception response tại đây */
					}
				} else {
					UART2_SendString("Unsupported function\r\n");
					/* Có thể thêm exception response tại đây */
				}

				/* ===== 2.5 Reset buffer ===== */
				rx_len = 0;
			}
		}
	}

	return 0;
}

void USART1_IRQHandler(void) {
	uint32_t sr = USART1->SR;

	// Handle errors: FE/NE/ORE -> clear by SR read then DR read
	if (sr & (USART_SR_FE | USART_SR_NE | USART_SR_ORE)) {
		volatile uint32_t tmp = USART1->DR;
		(void) tmp;
		rx_len = 0;               // drop current frame
		neFlag = (sr & USART_SR_NE) ? 1 : 0;
		frameFlag = (sr & USART_SR_FE) ? 1 : 0;
		return;
	}

	// RX data
	if (sr & USART_SR_RXNE) {
		uint8_t b = (uint8_t) USART1->DR;
		if (rx_len < RX_MAX)
			rx_buf[rx_len++] = b;
		else
			rx_len = 0;          // overflow -> reset
	}

	// IDLE line => end of frame
	if (sr & USART_SR_IDLE) {
		volatile uint32_t tmp = USART1->DR;
		(void) tmp;  // clear IDLE by SR/DR sequence
		if (rx_len > 0)
			frame_ready = 1;
	}
}

/*void USART1_IRQHandler(void) {
 char received_slave_address = 0;

 *
 * If there is framing error (physical) in the modbus, we raise a flag here. Since inside
 * IRQ handler actions should be simple and fast.  We will then investigate the flag in main()

 if (USART1->SR & USART_SR_FE) {
 frameFlag = 1;
 }

 if (USART1->SR & USART_SR_FE)
 {
 volatile uint32_t tmp;
 tmp = USART1->SR;
 tmp = USART1->DR;
 (void)tmp;

 frameFlag = 1;
 }

 *
 * If there is noise error (physical) in the modbus, we raise a flag here. Since inside
 * IRQ handler actions should be simple and fast. We will then investigate the flag in main()

 if (USART1->SR & USART_SR_NE) {
 neFlag = 1;
 }

 if (USART1->SR & 0x0020) 		//if data available in DR register. p737
 {
 received_slave_address = USART1->DR;
 }
 if (received_slave_address == SLAVE_ADDRESS) //if we have right address
 {
 mFlag = 1;

 } else {
 mFlag = 2;

 }
 USART1->CR1 &= ~0x0020;			//disable RX interrupt

 }*/

void wrong_slave_address(void) {
	USART1->CR1 &= ~0x00000004;		//RE bit. p739-740. Disable receiver
	delay_ms_tim6(10); 				//time=1/9600 x 10 bits x 7 byte = 7,29 ms
	USART1->CR1 |= 0x00000004;		//RE bit. p739-740. Enable receiver
	USART1->CR1 |= 0x0020;			//enable RX interrupt
	mFlag = 0;
}

void read_7_bytes_from_usartx(char *received_frame) {
	char frame[8] = { 0 };
	uint8_t i = 0;

	while (i < 7) {
		*received_frame = USART1_read();
		frame[i] = *received_frame;
		received_frame++;
		i++;
	}
	write_debug_frame(frame, 8);
}

int read_sensor(int input_address) {

	return 50;
}

void respond_frame(int sensor_value) {
	GPIOA->ODR |= 0x20;				//led on, transmitting mode
	//example response should be like this: 0104020B057E03
	char respond_frame[7] = { SLAVE_ADDRESS, 0x04, 0x02, 0, 0, 0, 0 };
	char sensor_high_bits = 0;
	char sensor_low_bits = 0;
	char crc_high_byte = 0;
	char crc_low_byte = 0;
	unsigned short int crc = 0; //16 bits

	sensor_high_bits = (sensor_value >> 8) | sensor_high_bits;
	sensor_low_bits = sensor_value | sensor_low_bits;
	respond_frame[3] = sensor_high_bits;
	respond_frame[4] = sensor_low_bits;
	crc = CRC16(respond_frame, 5);
	crc_high_byte = (crc >> 8) | crc_high_byte; //high byte
	crc_low_byte = crc | crc_low_byte; //low byte

	respond_frame[6] = crc_high_byte;
	respond_frame[5] = crc_low_byte;

	for (int i = 0; i < 7; i++) {
		USART1_write(respond_frame[i]);
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
void write_debug_frame(char *str, int maxchars) {
	int i;
	for (i = 0; i < 8; i++) {
		UART2_SendChar('0');
		UART2_SendChar('x');
		UART2_SendChar(bytestr[(str[i] & 0xf0) >> 4]);
		UART2_SendChar(bytestr[str[i] & 0x0f]);
		UART2_SendChar(44);
	}
	UART2_SendChar('\r');
	UART2_SendChar('\n');
}

