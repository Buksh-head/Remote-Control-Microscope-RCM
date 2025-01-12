 /** 
 **************************************************************
 * @file mylib/s4743556_rcmdisplay.c
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_rcmdisplay_init() - creates display t
 ***************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "s4743556_rcmdisplay.h"
#include "debug_log.h"
#include "board.h"
#include "s4743556_lta1000g.h"
#include "s4743556_rgb.h"
#include "s4743556_ssd.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

typedef struct {
    int x; 
    int y; 
    int rotateValue;
    int movedZ;
    int Zvalue;
    bool grid;
    bool xMoved;
    bool yMoved;
} rcmPos;

static rcmPos previous = {X_OFFSET, Y_OFFSET, 0, 0, 0, 0, false, false};
static char grid[GRID_SIZE][GRID_SIZE];
char empty[] = "|. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .|";
char border[] = "|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|";

void update_grid_output(rcmPos position, char symbol) {
    char buffer[OUTPUT_BUFFER_SIZE + X_OFFSET];  // Buffer for output
    snprintf(buffer, OUTPUT_BUFFER_SIZE + X_OFFSET, "\e[%d;%dH%c", 
             position.y, 
             position.x,
             symbol);
    BRD_debuguart_putm((unsigned char *)buffer, strlen(buffer));  // Send through UART
}

void s4743556_rcmdisplay_init() {
    BRD_debuguart_init();  // Initialise UART
    s4743556_ssd_init();
    s4743556_reg_rgb_init();
    s4743556_lta1000g_init();
    S4743556_REG_RGB_RED();
    s4743556_reg_lta1000g_write(0);
    char clear[] = "\e[2J";  // Clear screen
    BRD_debuguart_putm((unsigned char *)clear, strlen(clear));
    char home[] = "\e[H";  // Move cursor to home
    BRD_debuguart_putm((unsigned char *)home, strlen(home));

    BRD_debuguart_putm((unsigned char *)border, strlen(border));
    BRD_debuguart_putc('\n');
    BRD_debuguart_putc('\r');
    for (int i = 0; i < 101; i++) {
        BRD_debuguart_putm((unsigned char *)empty, strlen(empty));
        BRD_debuguart_putc('\n');
        BRD_debuguart_putc('\r');
    }
    BRD_debuguart_putm((unsigned char *)border, strlen(border));
    BRD_debuguart_putc('\n');
    BRD_debuguart_putc('\r');
    update_grid_output(previous, '+');
}

/**
* Receives the position of the RCM and displays it on the console  
*/
void s4743556_rcmdisplay_task() {
    taskENTER_CRITICAL();
    s4743556_rcmdisplay_init();
    taskEXIT_CRITICAL();
    rcmPos rcmPos;
    s4743556rcmQueuePosition = xQueueCreate(10, sizeof(rcmPos));

    for (;;) {
        if (xQueueReceive(s4743556rcmQueuePosition, &rcmPos, 10) == pdTRUE) {
            if (rcmPos.grid) {
                update_grid_output(previous, '.');

                rcmPos.x = X_OFFSET + rcmPos.x;
                rcmPos.y = Y_OFFSET - rcmPos.y/2;

                update_grid_output(rcmPos, '+'); 

                previous = rcmPos;
            } else {
                s4743556_reg_lta1000g_write(rcmPos.rotateValue);
                if (rcmPos.movedZ < 0) {
                    S4743556_REG_RGB_BLUE();
                } else if (rcmPos.movedZ > 0) {
                    S4743556_REG_RGB_GREEN();
                } 
                if (rcmPos.Zvalue == Z_MAX) {
                    S4743556_REG_RGB_RED();
                }
            }
        }
        if (rcmPos.xMoved) {
            s4743556_ssd_set(3,(rcmPos.x - X_OFFSET) / 100); // to get the hundreds digit
            vTaskDelay(3);
            s4743556_ssd_set(2,((rcmPos.x - X_OFFSET) / 10) % 10); // to get the tens digit
            vTaskDelay(3);
            s4743556_ssd_set(1,(rcmPos.x - X_OFFSET) % 10); // to get the ones digit
        } else if (rcmPos.yMoved) {
            s4743556_ssd_set(3,(-rcmPos.y + Y_OFFSET) / 50);
            vTaskDelay(3);
            s4743556_ssd_set(2,((-rcmPos.y + Y_OFFSET) / 5) % 10);
            vTaskDelay(3);
            s4743556_ssd_set(1,(-rcmPos.y + Y_OFFSET) * 2 % 10);
        } else if (rcmPos.movedZ){
            s4743556_ssd_set(2,(rcmPos.Zvalue / 10) % 10);
            vTaskDelay(3);
            s4743556_ssd_set(1,rcmPos.Zvalue % 10);
        } else {
            s4743556_ssd_clear();
        }
        vTaskDelay(3);
    }
}

/**
 * Creates display task
 */
void s4743556_tsk_rcmdisplay_init() {
    xTaskCreate((void *) &s4743556_rcmdisplay_task, (const signed char * const) "RCM Control", DISPLAY_STACK_SIZE, NULL, PRIORITY_DISPLAY, NULL);
}

