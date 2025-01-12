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

#include "board.h"
#include "processor_hal.h"

extern void s4743556_reg_mfs_led_init();

#define S4743556_REG_MFS_LED_D1_OFF() GPIOA->ODR |= (0X01 << 5)
#define S4743556_REG_MFS_LED_D2_OFF() GPIOA->ODR |= (0X01 << 6)
#define S4743556_REG_MFS_LED_D3_OFF() GPIOA->ODR |= (0X01 << 7)
#define S4743556_REG_MFS_LED_D4_OFF() GPIOD->ODR |= (0X01 << 14)
#define S4743556_REG_MFS_LED_D1_ON() GPIOA->ODR &= ~(0X01 << 5)
#define S4743556_REG_MFS_LED_D2_ON() GPIOA->ODR &= ~(0X01 << 6)
#define S4743556_REG_MFS_LED_D3_ON() GPIOA->ODR &= ~(0X01 << 7)
#define S4743556_REG_MFS_LED_D4_ON() GPIOD->ODR &= ~(0X01 << 14)