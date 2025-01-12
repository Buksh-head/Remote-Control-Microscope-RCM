 /** 
 **************************************************************
 * @file mylib/s4743556_brd_pb.h
 * @author Adnaan Buksh - 47435568
 * @date 28042024
 * @brief brd Pushbutton Mylib Register Driver Header File
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_brd_pb_init () - intialise mylib template driver
 * s4743556_reg_brd_pb_press_get () - return press counter
 * s4743556_reg_brd_pb_press_reset () - set press counter to zero
 ***************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t s4743556brdSemaphorePBSig;

extern void s4743556_reg_brd_pb_init ();