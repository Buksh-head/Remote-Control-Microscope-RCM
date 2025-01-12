 /** 
 **************************************************************
 * @file mylib/s4743556_rgb.c
 * @author Adnaan Buksh - 47435568
 * @date 01032024
 * @brief RGB Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_rgb_init () - intialise mylib template driver
 * s4743556_reg_rgb_brightness_write () - Contorols brightness
 * s4743556_reg_rgb_brightness_read () - Read brightness
 * s4743556_reg_rgb_colour_set () - Sets colour of LED
 * s4743556_reg_rgb_colour_get () - returns colour of LED
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

/*
 * Define functions for primary colours using #define
 */
#define S4743556_REG_RGB_BLACK()   s4743556_reg_rgb_colour_set(0x07)
#define S4743556_REG_RGB_BLUE()    s4743556_reg_rgb_colour_set(0x06)
#define S4743556_REG_RGB_GREEN()   s4743556_reg_rgb_colour_set(0x05)
#define S4743556_REG_RGB_CYAN()    s4743556_reg_rgb_colour_set(0x04)
#define S4743556_REG_RGB_RED()     s4743556_reg_rgb_colour_set(0x03)
#define S4743556_REG_RGB_MAGENTA() s4743556_reg_rgb_colour_set(0x02)
#define S4743556_REG_RGB_YELLOW()  s4743556_reg_rgb_colour_set(0x01)
#define S4743556_REG_RGB_WHITE()   s4743556_reg_rgb_colour_set(0x00)

#define TIMER_COUNTER_FREQ 10000 //Frequency (in Hz)
#define PWM_PULSE_WIDTH_TICKS				200 					//2*((PWM_PULSE_WIDTH_SECONDS)/(1/TIMER_COUNTER_FREQ))		//Period in timer ticks
#define PWM_PULSE_WIDTH_SECONDS				0.005						

#define PWM_PERCENT2TICKS_DUTYCYCLE(value)	(uint16_t)(value*PWM_PULSE_WIDTH_TICKS/100)				//Duty Cycle on time in timer ticks
#define PWM_DUTYCYCLE_CHANGE(value) 			TIM1->CCR1=(uint16_t)value	

extern void s4743556_reg_rgb_init();
extern void s4743556_reg_rgb_brightness_write(int level);
extern int s4743556_reg_rgb_brightness_read();
extern void s4743556_reg_rgb_colour_set(unsigned char rgb_mask);
extern unsigned char s4743556_reg_rgb_colour_get();