 /** 
 **************************************************************
 * @file mylib/s4743556_rcmconsole.h
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_rcmdisplay_init() - creates display task
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// Task Priorities (Idle Priority is the lowest priority)
#define PRIORITY_DISPLAY					(tskIDLE_PRIORITY + 3)

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define DISPLAY_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

#define Y_OFFSET 102
#define X_OFFSET 2
#define Z_MAX 99
#define GRID_SIZE 100
#define OUTPUT_BUFFER_SIZE 10

extern QueueHandle_t s4743556rcmQueuePosition;

extern void s4743556_tsk_rcmdisplay_init();
