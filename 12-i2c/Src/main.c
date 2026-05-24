/* testing I2C driver on
 * sensor AHT20
 */

#include "main.h"

#define AHT20_ADDR_7B   0x38
#define AHT20_ADDR_W    (AHT20_ADDR_7B << 1)        // 0x70
#define AHT20_ADDR_R    ((AHT20_ADDR_7B << 1) | 1)  // 0x71

#define CMD_STATUS      0x71
#define CMD_MEASURE     0xAC
#define MEAS_P1         0x33
#define MEAS_P2         0x00

static char buf[100];

uint8_t AHT20_ReadStatus(void);

void AHT20_TriggerMeasurement(void);

uint8_t AHT20_ReadData(uint8_t *data);

float AHT20_GetHumidity(uint8_t *data);

float AHT20_GetTemperature(uint8_t *data);

/* =========================
 * MAIN
 * ========================= */

int main(void) {

	SystemClock_Config();
	SystemCoreClockUpdate();
	fpu_enable();

	Uart2_init();
	Tim6_init();
	I2C_Config();

	uint8_t rxData[6];

	float temperature;
	float humidity;

	UART2_SendString("AHT20 Start...\r\n");

	delay_ms_tim6(100);

	// check calibration bit
	uint8_t status = AHT20_ReadStatus();

	sprintf(buf, "Status: 0x%02X\r\n", status);
	UART2_SendString(buf);

	if ((status & 0x08) == 0) {
		UART2_SendString("AHT20 NOT calibrated!\r\n");
	} else {
		UART2_SendString("AHT20 calibrated OK\r\n");
	}

	while (1) {

		if (AHT20_ReadData(rxData)) {

			humidity = AHT20_GetHumidity(rxData);
			temperature = AHT20_GetTemperature(rxData);

			sprintf(buf, "Temp: %.2f C   Humidity: %.2f %%RH\r\n", temperature,
					humidity);

			UART2_SendString(buf);
		}

		// datasheet recommends >1 second
		delay_ms_tim6(2000);
	}
}

/* =========================
 * AHT20 LOW LEVEL
 * ========================= */

uint8_t AHT20_ReadStatus(void) {

	uint8_t status;

	// START condition
	I2C_Start();

	// Send sensor address (WRITE)
	I2C_Address(AHT20_ADDR_W);

	// Send status command (0x71)
	I2C_Write(CMD_STATUS);

	// Repeated START (keep bus active, switch to READ)
	I2C_Start();

	// Read 1 byte status
	I2C_Read(AHT20_ADDR_R, &status, 1);

	// STOP condition (release bus)
	I2C_Stop();

	return status;
}

void AHT20_TriggerMeasurement(void) {

	uint8_t cmd[3];

	// Build measurement command sequence:
	// 0xAC = trigger measurement, then parameters 0x33, 0x00
	cmd[0] = CMD_MEASURE;
	cmd[1] = MEAS_P1;
	cmd[2] = MEAS_P2;

	// START condition
	I2C_Start();

	// Send sensor address in WRITE mode
	I2C_Address(AHT20_ADDR_W);

	// Write all 3 bytes (command + parameters)
	I2C_WriteMulti(cmd, 3);

	// STOP condition (release the bus)
	I2C_Stop();
}

uint8_t AHT20_ReadData(uint8_t *data) {

	uint8_t status;

	// Tell sensor to start a new measurement
	AHT20_TriggerMeasurement();

	// Wait a short time for measurement to begin
	delay_ms_tim6(100);

	// Poll status until BUSY bit (bit7) becomes 0
	do {
		status = AHT20_ReadStatus();
		delay_ms_tim6(10);
	} while (status & 0x80);

	// START condition
	I2C_Start();

	// Read 6 bytes of measurement data (status + humidity + temperature bytes)
	I2C_Read(AHT20_ADDR_R, data, 6);
	I2C_Stop();

	// Return success (can be changed later to real error handling)
	return 1;
}

/* =========================
 * DATA CONVERSION
 * ========================= */

float AHT20_GetHumidity(uint8_t *data) {

	uint32_t rawHumidity;

	// Humidity is a 20-bit value:
	// data[1] = bits 19:12
	// data[2] = bits 11:4
	// data[3](upper nibble) = bits 3:0
	rawHumidity = ((uint32_t) data[1] << 12) | ((uint32_t) data[2] << 4)
			| ((data[3] & 0xF0) >> 4);

	// Convert raw 20-bit value to %RH (0..100)
	float humidity = ((float) rawHumidity * 100.0f) / 1048576.0f;

	return humidity;
}

float AHT20_GetTemperature(uint8_t *data) {

	uint32_t rawTemp;

	// Temperature is a 20-bit value:
	// data[3](lower nibble) = bits 19:16
	// data[4] = bits 15:8
	// data[5] = bits 7:0
	rawTemp = (((uint32_t) (data[3] & 0x0F)) << 16) | ((uint32_t) data[4] << 8)
			| (uint32_t) data[5];

	// Convert raw 20-bit value to °C using datasheet formula
	float temperature = ((float) rawTemp * 200.0f / 1048576.0f) - 50.0f;

	return temperature;
}
