 /** 
 **************************************************************
 * @file mylib/s4743556_rcmcont.h
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_rcmcontrol_init() - creates control task
 ***************************************************************
 */

#include "board.h"
#include "processor_hal.h"

// Task Priorities (Idle Priority is the lowest priority)
#define PRIORITY_CONTROL					(tskIDLE_PRIORITY + 1)

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define CONTROL_STACK_SIZE		( configMINIMAL_STACK_SIZE * 3 )

#define Z_MAX 99
#define Y_MAX 200
#define X_MAX 200
#define ZOOM_MAX 9
#define ZOOM_MIN 1
#define ROTATE_MAX 180
#define ROTATE_MIN 0
#define ROTATE_CODE 0x23
#define ZOOM_CODE 0x25
#define MOVE_CODE 0x22
#define PAYLOAD_BUF 12
#define BUFFER 16
#define SMALL_MOVE 2
#define MEDIUM_MOVE 10
#define LARGE_MOVE 50
#define ZOOM_IN 1
#define ZOOM_OUT -1
#define ROTATE_LEFT 10
#define ROTATE_RIGHT -10

extern void s4743556_tsk_rcmcontrol_init();
