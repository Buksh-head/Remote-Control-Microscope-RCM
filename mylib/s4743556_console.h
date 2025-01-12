 /** 
 **************************************************************
 * @file mylib/s4743556_console.c
 * @author Adnaan Buksh - 47435568
 * @date 20032024
 * @brief Driver processes input, received via a console input
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_lib_console_ascii2hex() - Convert an ASCII hexadecimal value
 * void s4743556_tsk_rcmconsole_init() - Creates console task
 ***************************************************************
 */
#define FREERTOS

#include "board.h"
#include "processor_hal.h"

extern int s4743556_lib_console_ascii2hex(char value);

#ifdef FREERTOS

// Task Priorities (Idle Priority is the lowest priority)
#define PRIORITY_CONSOLE					(tskIDLE_PRIORITY + 2)

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define CONSOLE_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

#define BOTTOM_ROW 104
#define KEYPRESS_Q (1 << 0)
#define KEYPRESS_W (1 << 1)
#define KEYPRESS_E (1 << 2)
#define KEYPRESS_R (1 << 3)
#define KEYPRESS_T (1 << 4)
#define KEYPRESS_Y (1 << 5)
#define KEYPRESS_A (1 << 6)
#define KEYPRESS_S (1 << 7)
#define KEYPRESS_D (1 << 8)
#define KEYPRESS_F (1 << 9)
#define KEYPRESS_G (1 << 10)
#define KEYPRESS_H (1 << 11)
#define KEYPRESS_Z (1 << 12)
#define KEYPRESS_X (1 << 13)
#define KEYPRESS_C (1 << 14)
#define KEYPRESS_V (1 << 15)
#define KEYPRESS_B (1 << 16)
#define KEYPRESS_N (1 << 17)
#define KEYPRESS_1 (1 << 18)
#define KEYPRESS_2 (1 << 19)
#define KEYPRESS_3 (1 << 20)
#define KEYPRESS_4 (1 << 21)
#define KEYPRESS_5 (1 << 22)
#define KEYPRESS_INVALID (1 << 23)
#define KEYPRESS_EVENT (KEYPRESS_Q | KEYPRESS_W | KEYPRESS_E | KEYPRESS_R | KEYPRESS_T | KEYPRESS_Y | KEYPRESS_A | KEYPRESS_S | KEYPRESS_D | KEYPRESS_F | KEYPRESS_G | KEYPRESS_H | KEYPRESS_Z | KEYPRESS_X | KEYPRESS_C | KEYPRESS_V | KEYPRESS_B | KEYPRESS_N | KEYPRESS_1 | KEYPRESS_2 | KEYPRESS_3 | KEYPRESS_4 | KEYPRESS_5 | KEYPRESS_INVALID)

extern void s4743556_tsk_rcmconsole_init();

#endif
