 /** 
 **************************************************************
 * @file mylib/s4743556_lta1000g.c
 * @author Adnaan Buksh - 47435568
 * @date 21022024
 * @brief LTA1000G LED Bar Display Mylib Register Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_lta1000g_init() - intialise mylib template driver
 * s4743556_lta1000g_set() - set GPIO pin high or low
 * s4743556_reg_lta1000g_write() - Write the LED Bar segments high or low
 * s4743556_reg_lta1000g_invert() - Inverts the LED Bar segments
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"

extern void s4743556_lta1000g_init ();
extern void lta1000g_seg_set (int segment, unsigned char value);
extern void s4743556_reg_lta1000g_write (unsigned short value);
extern void s4743556_reg_lta1000g_invert (unsigned short value, unsigned short mask);