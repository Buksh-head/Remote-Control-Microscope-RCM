 /** 
 **************************************************************
 * @file mylib/s4743556_mfs_pb.h
 * @author Adnaan Buksh - 47435568
 * @date 21022024
 * @brief MFS Pushbutton Mylib Register Driver Header File
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_mfs_pb_init () - intialise mylib template driver
 * s4743556_reg_mfs_pb_press_get () - return press counter
 * s4743556_reg_mfs_pb_press_reset () - set press counter to zero
 ***************************************************************
 */
//#define FreeRTOS

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"

#ifdef FreeRTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#endif

extern void s4743556_reg_mfs_pb_init (int pb_select);
extern int s4743556_reg_mfs_pb_press_get (int pb_select);
extern void s4743556_reg_mfs_pb_press_reset (int pb_select);