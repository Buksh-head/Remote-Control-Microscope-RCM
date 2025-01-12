 /** 
 **************************************************************
 * @file mylib/s4743556_mfs_trimpot.c
 * @author Adnaan Buksh - 47435568
 * @date 01032024
 * @brief MFS Trimpot Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_mfs_trimpot_init () - intialise mylib template driver
 * s4743556_reg_mfs_trimpot_get () -  current Trimpot input value
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

#define S4743556_REG_MFS_TRIMPOT_ZERO_CAL_OFFSET 0

extern void s4743556_reg_mfs_trimpot_init ();
extern int s4743556_reg_mfs_trimpot_get ();