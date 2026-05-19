/*
 * interrupt_user_button.c
 *
 *  Created on: 16.5.2026
 *      Author: nn
 */

#include "nucleo446start.h"
#include "interrupt_user_button.h"

void GPIO_Config(void) {

	RCC->AHB1ENR |= (1 << 2);  // Enable GPIOC clock

	GPIOC->MODER &= ~(3 << (2 * 13)); // Bits (3:2) = 0:0  --> PC13 in Input Mode

	GPIOC->PUPDR &= ~(3 << (2 * 13));  // No pull
}

void Interrupt_Config(void) {
	/*************>>>>>>> STEPS FOLLOWED <<<<<<<<************

	 1. Enable the SYSCNFG bit in RCC register
	 2. Configure the EXTI configuration Regiter in the SYSCNFG
	 3. Enable the EXTI using Interrupt Mask Register (IMR)
	 4. Configure the Rising Edge / Falling Edge Trigger
	 5. Set the Interrupt Priority
	 6. Enable the interrupt

	 ********************************************************/

	RCC->APB2ENR |= (1 << 14);  // Enable SYSCNFG

	SYSCFG->EXTICR[3] &= ~(0xf << 4); // Bits[7:6:5:4] = (0:0:0:0)  -> clear EXTI13 line for PC13
	SYSCFG->EXTICR[3] |= 2 << 4; // Bits[7:6:5:4] = (0:0:1:0)  -> configure EXTI13 line for PC13

	EXTI->IMR |= (1 << 13);  // Bit[13] = 1  --> Disable the Mask on EXTI13

	EXTI->RTSR &= ~(1 << 13);  // Disable Rising Edge Trigger for PC13

	EXTI->FTSR |= (1 << 13);  // Enable Falling Edge Trigger for PC13

	NVIC_SetPriority(EXTI15_10_IRQn, 0);  // Set Priority

	NVIC_EnableIRQ(EXTI15_10_IRQn);  // Enable Interrupt

}
