 /** 
 **************************************************************
 * @file mylib/s4743556_mfs_led.c
 * @author Adnaan Buksh - 47435568
 * @date 20032024
 * @brief controls the MFS LEDs D1 to D4
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_mfs_led_init() - initialise MFS LED
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_mfs_led.h"

/**
 * initialise MFS LED
 */
void s4743556_reg_mfs_led_init() {
    __GPIOA_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    //D1
    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << 5);   // Set fast speed for PA5.
    GPIOA->MODER &= ~(0x03 << (5 * 2));         // Clear bits for output on PA5.
    GPIOA->MODER |= (0x01 << (5 * 2));          // Set bits for output on PA5.
    GPIOA->OTYPER &= ~(0x01 << 5);             // Clear bits for push-pull

    //D2
    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << 6);   
    GPIOA->MODER &= ~(0x03 << (6 * 2)); 
    GPIOA->MODER |= (0x01 << (6* 2));  
    GPIOA->OTYPER &= ~(0x01 << 6);

    //D3
    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << 7);    
    GPIOA->MODER &= ~(0x03 << (7 * 2)); 
    GPIOA->MODER |= (0x01 << (7 * 2));  
    GPIOA->OTYPER &= ~(0x01 << 7);

    //D4
    GPIOD->OSPEEDR |= (GPIO_SPEED_FAST << 14);         
    GPIOD->MODER &= ~(0x03 << (14 * 2)); 
    GPIOD->MODER |= (0x01 << (14 * 2));  
    GPIOD->OTYPER &= ~(0x01 << 14);
}

