 /** 
 **************************************************************
 * @file mylib/s4743556_sysmon.c
 * @author Adnaan Buksh - 47435568
 * @date 10042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern void s4743556_sysmon_init();

//System monitor pins
#define SYSMON_CHAN0_PIN		10
#define SYSMON_CHAN0_GPIO		GPIOD
#define SYSMON_CHAN0_GPIO_CLK()	__GPIOD_CLK_ENABLE()

#define SYSMON_CHAN1_PIN		7
#define SYSMON_CHAN1_GPIO		GPIOG
#define SYSMON_CHAN1_GPIO_CLK()	__GPIOG_CLK_ENABLE()

#define SYSMON_CHAN2_PIN		4
#define SYSMON_CHAN2_GPIO		GPIOG
#define SYSMON_CHAN2_GPIO_CLK()	__GPIOG_CLK_ENABLE()

//System monitor macros
#define S4743556_REG_SYSMON_CHAN0_SET()		SYSMON_CHAN0_GPIO->ODR |= (0x01 << SYSMON_CHAN0_PIN);
#define S4743556_REG_SYSMON_CHAN0_CLR()		SYSMON_CHAN0_GPIO->ODR &= ~(0x01 << SYSMON_CHAN0_PIN);
#define S4743556_REG_SYSMON_CHAN1_SET()		SYSMON_CHAN1_GPIO->ODR |= (0x01 << SYSMON_CHAN1_PIN);
#define S4743556_REG_SYSMON_CHAN1_CLR()		SYSMON_CHAN1_GPIO->ODR &= ~(0x01 << SYSMON_CHAN1_PIN);
#define S4743556_REG_SYSMON_CHAN2_SET()		SYSMON_CHAN2_GPIO->ODR |= (0x01 << SYSMON_CHAN2_PIN);
#define S4743556_REG_SYSMON_CHAN2_CLR()		SYSMON_CHAN2_GPIO->ODR &= ~(0x01 << SYSMON_CHAN2_PIN);