 /** 
 **************************************************************
 * @file mylib/s4743556_txradio.h
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_txradio_init() - creates txradio task
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

// Task Priorities (Idle Priority is the lowest priority)
#define PRIORITY_RADIO					(tskIDLE_PRIORITY + 2)

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define RADIO_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

#define BUFFER_SIZE 16

extern QueueHandle_t s4743556radioTxQueue;
extern uint8_t pbPressed;

extern void s4743556_tsk_txradio_init();
void s4743556_txradio_init();
void s4743556_txradio_task();
