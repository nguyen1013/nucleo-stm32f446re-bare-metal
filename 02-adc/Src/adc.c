/*
 * adc.c
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#include "adc.h"

/* For ACD1 in PA0 and PA1 and PA4 */
void ADC_Init(void)
{
    // 1. Clock
    RCC->APB2ENR |= (1 << 8);  // ADC1
    RCC->AHB1ENR |= (1 << 0);  // GPIOA

    // 2. Prescaler
    ADC->CCR |= (1 << 16);     // PCLK2 /4

    // 3. Set the Scan Mode and Resolution in the Control Register 1 (CR1)
    ADC1->CR1 |= (1 << 8);     // SCAN
    ADC1->CR1 &= ~(3 << 24);   // 12-bit

    // 4. Set the Continuous Conversion, EOC, and Data Alignment in Control Reg 2 (CR2)
    ADC1->CR2 |= (1 << 1);     // CONT
    ADC1->CR2 |= (1 << 10);    // EOC each conversion
    ADC1->CR2 &= ~(1 << 11);   // right align

    // 5. Sampling time
    ADC1->SMPR2 &= ~((7 << 0) | (7 << 3) | (7 << 12));
    ADC1->SMPR2 |=  ((0 << 0) | (0 << 3) | (0 << 12));

    // 6. Set the Regular channel sequence length = 3 in ADC_SQR1
    ADC1->SQR1 &= ~(0xF << 20);
    ADC1->SQR1 |=  (2 << 20);

    // 7. GPIO analog
    GPIOA->MODER |= (3 << 0);  // PA0
    GPIOA->MODER |= (3 << 2);  // PA1
    GPIOA->MODER |= (3 << 8);  // PA4

}

void ADC_Enable(void) {
	/************** STEPS TO FOLLOW *****************
	 1. Enable the ADC by setting ADON bit in CR2
	 2. Wait for ADC to stabilize
	 ************************************************/
	ADC1->CR2 |= 1 << 0;   // ADON =1 enable ADC1

	uint32_t delay = 10000;
	while (delay--)
		;
}

void ADC_Start(int channel) {
	/************** STEPS TO FOLLOW *****************
	 1. Set the channel Sequence in the SQR Register
	 2. Clear the Status register
	 3. Start the Conversion by Setting the SWSTART bit in CR2
	 ************************************************/

	/**	Polling for each channel, keep one channel in the sequence at a time
	 ADC1->SQR3 |= (channel<<0); will just keep the respective channel in the sequence for the conversion **/

	ADC1->SQR3 = 0;
	ADC1->SQR3 |= (channel << 0);    // conversion in regular sequence

	ADC1->SR = 0;        // clear the status register

	ADC1->CR2 |= (1 << 30);  // start the conversion
}

void ADC_WaitForConv(void) {
	/*************************************************
	 EOC Flag will be set, once the conversion is finished
	 *************************************************/
	while (!(ADC1->SR & (1 << 1)))
		;  // wait for EOC flag to set
}

uint16_t ADC_GetVal(void) {
	return ADC1->DR;  // Read the Data Register
}

void ADC_Disable(void) {
	/************** STEPS TO FOLLOW *****************
	 1. Disable the ADC by Clearing ADON bit in CR2
	 ************************************************/
	ADC1->CR2 &= ~(1 << 0);  // Disable ADC
}
