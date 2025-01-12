 /** 
 **************************************************************
 * @file mylib/s4743556_mfs_pb.c
 * @author Adnaan Buksh - 47435568
 * @date 21022024
 * @brief MFS Pushbutton Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_mfs_pb_init () - intialise mylib template driver
 * s4743556_reg_mfs_pb_press_get () - return press counter
 * s4743556_reg_mfs_pb_press_reset () - set press counter to zero
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_mfs_pb.h"
#include <stdbool.h>

#ifdef FreeRTOS
extern SemaphoreHandle_t SemaphorePBSig;
#endif

#define S4743556_REG_MFA_PB_S1 1 << 0
#define S4743556_REG_MFA_PB_S2 1 << 1 
#define S4743556_REG_MFA_PB_S3 1 << 2 
#define S4743556_REG_MFA_PB_S4 1 << 3

/*
 * holds current MFS pressed count for each MFS pushbutton
 */
static int MfPbPressCounter[4];

/*
 * Last system time on when button was pressed
 */
static unsigned int lastPressTime = 0;

/*
 * State of button
 */
bool pressed = false;

/*
 * Enabled the specified pushbutton source, enables GPIO input and interrupt.
 */
void s4743556_reg_mfs_pb_init (int pbSelect) {
    // Enable GPIO Clock
    __GPIOC_CLK_ENABLE();

    GPIOC->OSPEEDR |= (GPIO_SPEED_FAST << 0);   // Set fast speed for PC0.
    GPIOC->PUPDR &= ~(0x03 << (0 * 2));         // Clear bits for no PUPD
    GPIOC->MODER &= ~(0x03 << (0 * 2));         // Clear bits for input on PC0.

    // Enable EXTI clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;    

	//select trigger source (port c, pin 0) on EXTICR4.
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;

	EXTI->RTSR |= EXTI_RTSR_TR0;	//enable rising edge
	EXTI->FTSR |= EXTI_FTSR_TR0;	//enable falling edge
	EXTI->IMR |= EXTI_IMR_IM0;		//Enable external interrupt

	//Enable priority (10) and interrupt callback. Do not set a priority < 5.
	HAL_NVIC_SetPriority(EXTI0_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*
 * MFS Pushbutton Interrupt service routine. Must be called from the 
 * corresponding GPIO EXTI interrupt service routine for the pushbutton used. 
 * Must implement pushbutton debouncing.
 */
void s4743556_reg_mfs_pb_isr (int pbSelect) {
    if ((HAL_GetTick() - lastPressTime) > 50 && (!pressed)) {
        if ((GPIOC->IDR & (0x01 << 0)) == (0x00 << 0)) {
            pressed = true;
            MfPbPressCounter[pbSelect]++; 
        }

        #ifdef FreeRTOS
		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;

		if (SemaphorePBSig != NULL) {	// Check if semaphore exists 
			xSemaphoreGiveFromISR(SemaphorePBSig, &xHigherPriorityTaskWoken);		// Give PB Semaphore from ISR
		}

		// Perform context switching, if required.
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		#endif

    } else {
        pressed = false;
    }
    lastPressTime = HAL_GetTick();
}

/*
 * Returns the value of the specified MFS pushbutton press counter
 */
int s4743556_reg_mfs_pb_press_get (int pbSelect) {
    return MfPbPressCounter[pbSelect];
}

/*
 * Reset the MFS pushbutton event counter value to 0, for the 
 * specified pushbutton.
 */
void s4743556_reg_mfs_pb_press_reset (int pbSelect) {
    MfPbPressCounter[pbSelect] = 0;
}

/*
 * Interrupt handler (ISR) for EXTI 0 IRQ Handler
 * Note ISR should only execute a callback
 */
void EXTI0_IRQHandler(void) {
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    // PR: Pending register
    if ((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) {
        // Cleared by writing a 1 to this bit
        EXTI->PR |= EXTI_PR_PR0;  // Clear interrupt flag.
        s4743556_reg_mfs_pb_isr(1); 
    }
}