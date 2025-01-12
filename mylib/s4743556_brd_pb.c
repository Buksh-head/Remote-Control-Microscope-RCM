 /** 
 **************************************************************
 * @file mylib/s4743556_brd_pb.c
 * @author Adnaan Buksh - 47435568
 * @date 28042024
 * @brief BRD Pushbutton Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_brd_pb_init () - intialise mylib template driver
 * s4743556_reg_brd_pb_press_get () - return press counter
 * s4743556_reg_brd_pb_press_reset () - set press counter to zero
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_brd_pb.h"
#include <stdbool.h>

/*
 * Last system time on when button was pressed
 */
static unsigned int lastPressTime = 0;

/*
 * State of button
 */
bool pressedBrd = false;

/*
 * Enabled the specified pushbutton source, enables GPIO input and interrupt.
 */
void s4743556_reg_brd_pb_init () {
    // Enable GPIO Clock
    __GPIOC_CLK_ENABLE();

    GPIOC->OSPEEDR |= (GPIO_SPEED_FAST << 13);   // Set fast speed for PC13.
    GPIOC->PUPDR &= ~(0x03 << (13 * 2));         // Clear bits for no PUPD
    GPIOC->MODER &= ~(0x03 << (13 * 2));         // Clear bits for input on PC13.

    // Enable EXTI clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//select trigger source (port c, pin 13) on EXTICR4.
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	EXTI->RTSR |= EXTI_RTSR_TR13;	//enable rising dedge
	EXTI->FTSR &= ~EXTI_FTSR_TR13;	//disable falling edge
	EXTI->IMR |= EXTI_IMR_IM13;		//Enable external interrupt

	//Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*
 * brd Pushbutton Interrupt service routine. Must be called from the 
 * corresponding GPIO EXTI interrupt service routine for the pushbutton used. 
 * Must implement pushbutton debouncing.
 */
void s4743556_reg_brd_pb_isr () {
    if ((HAL_GetTick() - lastPressTime) > 50 && (!pressedBrd)) {
        if ((GPIOC->IDR & (0x01 << 0)) == (0x00 << 0)) {
            pressedBrd = true;
        }

		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;

		if (s4743556brdSemaphorePBSig != NULL) {	// Check if semaphore exists 
			xSemaphoreGiveFromISR(s4743556brdSemaphorePBSig, &xHigherPriorityTaskWoken);		// Give PB Semaphore from ISR
		}

		// Perform context switching, if required.
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    } else {
        pressedBrd = false;
    }
    lastPressTime = HAL_GetTick();
}

/*
 * Interrupt handler (ISR) for EXTI 15 to 10 IRQ Handler
 * Note ISR should only execute a callback
 */
void EXTI15_10_IRQHandler(void) {
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    // PR: Pending register
    if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {
        // Cleared by writing a 1 to this bit
        EXTI->PR |= EXTI_PR_PR13;  // Clear interrupt flag.
        s4743556_reg_brd_pb_isr(); 
    }
}