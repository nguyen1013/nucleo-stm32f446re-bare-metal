/* Includes */
#include "main.h"
#include "modbus_crc.h"
#include "uart1_interrupt.h"

#define SLAVE_ADDRESS 0x01
#define INPUT_REGISTER 0x01

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



	char received_frame[8] = { 0 };
	/* Infinite loop */
	unsigned short int crc = 0; //16 bitts
	char crc_high_byte = 0;
	char crc_low_byte = 0;

	while (1) {
		if (frameFlag == 1) {
			/* Here we have encountered a framing erorr. I tshould not occur, but if it does, here we handle it */
			UART2_SendString("Framing Error Detected");
			frameFlag = 0;
			clear_buffer(received_frame);
		}
		if (neFlag == 1) {
			/* If we have noise error in communication, we handle it here */
			UART2_SendString("Noise Error Detected");
			neFlag = 0;
			clear_buffer(received_frame);
		}
		if (mFlag == 1) //correct slave address
				{
			read_7_bytes_from_usartx(&received_frame[1]);
			UART2_SendString("received frame....\n\r");
			write_debug_frame(received_frame, 8);
			received_frame[0] = SLAVE_ADDRESS;
			crc = CRC16(received_frame, 6);
			crc_high_byte = crc >> 8 | crc_high_byte; //high byte
			crc_low_byte = crc | crc_low_byte; //low byte

			if ((received_frame[7] == crc_high_byte)
					&& (received_frame[6] == crc_low_byte)) //CRC check
					{
				if (received_frame[3] == INPUT_REGISTER) //input register check and can be added more registers
				{
					int sensor_value = read_sensor(INPUT_REGISTER);
					respond_frame(sensor_value); //no multiplication by x10 because ADC value quite big 300-1000
				} else {
				}
			} else {
			}
			crc = 0;
			crc_low_byte = 0;
			crc_high_byte = 0;
			mFlag = 0;
			//GPIOA->ODR|=0x20;				//0010 0000 or bit 5. p186
			USART1->CR1 |= 0x0020;			//enable RX interrupt
		} else if (mFlag == 2) //wrong slave address
				{
			wrong_slave_address();

			received_frame[0] = 0;
		}
	}

	return 0;
}

void USART1_IRQHandler(void) {
	char received_slave_address = 0;

	/**
	 * If there is framing error (physical) in the modbus, we raise a flag here. Since inside
	 * IRQ handler actions should be simple and fast.  We will then investigate the flag in main()
	 */
/*	if (USART1->SR & USART_SR_FE) {
		frameFlag = 1;
	}*/

	if (USART1->SR & USART_SR_FE)
	 {
	 volatile uint32_t tmp;
	 tmp = USART1->SR;
	 tmp = USART1->DR;
	 (void)tmp;

	 frameFlag = 1;
	 }

	/**
	 * If there is noise error (physical) in the modbus, we raise a flag here. Since inside
	 * IRQ handler actions should be simple and fast. We will then investigate the flag in main()
	 */
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

}



void wrong_slave_address(void) {
	USART1->CR1 &= ~0x00000004;		//RE bit. p739-740. Disable receiver
	delay_ms_tim6(10); 					//time=1/9600 x 10 bits x 7 byte = 7,29 ms
	USART1->CR1 |= 0x00000004;		//RE bit. p739-740. Enable receiver
	USART1->CR1 |= 0x0020;			//enable RX interrupt
	mFlag = 0;
}

void read_7_bytes_from_usartx(char *received_frame)
{
	char frame[8]={0};
	uint8_t i=0;

	while(i<7)
	{
		*received_frame=USART1_read();
		frame[i]=*received_frame;
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

