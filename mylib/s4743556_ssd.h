 /** 
 **************************************************************
 * @file mylib/s4743556_ssd.h
 * @author Adnaan Buksh - 47435568
 * @date 10052024
 * @brief Seven Segment Display Mylib Register Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_ssd_init() - intialise mylib driver
 * s4743556_ssd_set() - set the 7 segment display to a number
 * s4743556_ssd_clear() - clear the 7 segment display
 ***************************************************************
 */

#include <stdint.h>
#include "board.h"
#include "processor_hal.h"

#define LATCH_PIN 14 //D4
#define CLK_PIN 13 //D7
#define DATA_PIN 12 //D8

extern void s4743556_ssd_init();
extern void s4743556_ssd_set();
extern void s4743556_ssd_clear();