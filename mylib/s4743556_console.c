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

#include <stdint.h>
#include <string.h>
#include "s4743556_console.h"

#ifdef FREERTOS
#include "s4743556_mfs_led.h"
#include "debug_log.h"
#include "s4743556_txradio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

extern EventGroupHandle_t s4743556rcmEventGroup;
#endif

/**
 * External function to convert an ASCII hexadecimal value
 * value to a (returned) binary hexadecimal e.g. ’C’ to 0xC
*/
int s4743556_lib_console_ascii2hex(char value) {
  if (value >= '0' && value <= '9') {
    return value - '0';
  } else if (value >= 'a' && value <= 'f') {
    return value - 'a' + 10;
  } else if (value >= 'A' && value <= 'F') {
    return value - 'A' + 10;
  } else {
    return -1;
  }
}

#ifdef FREERTOS
/**
 * Sets Event bits based on keypresses
 **/
void s4743556_rcmconsole_task() {
    char c;
    EventBits_t uxBits;
    char valid_chars[] = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'A', 'S', 'D', 'F', 'G', 'H', 
    'Z', 'X', 'C', 'V', 'B', 'N', '1', '2', '3', '4', '5'
    };

    for (;;) {
      c = debug_getc();
      char buffer[15];  // Buffer for output
      snprintf(buffer, 15, "\e[%d;%dH%c", BOTTOM_ROW, 0, c);
      BRD_debuguart_putm((unsigned char *)buffer, strlen(buffer));
      if (c != '\0') {
        int is_valid = 0;
        if (!pbPressed) {
          uxBits = xEventGroupSetBits(s4743556rcmEventGroup, KEYPRESS_INVALID);
          continue;
        }
        // Check if the character is in the valid_chars array
        for (size_t  i = 0; i < sizeof(valid_chars) / sizeof(valid_chars[0]); i++) {
          if (valid_chars[i] == c) {
            uxBits = xEventGroupSetBits(s4743556rcmEventGroup, (1 << i));
            is_valid = 1;
            break;
          }
        }
        // If the character is not found, set the invalid event bit
        if (!is_valid) {
          uxBits = xEventGroupSetBits(s4743556rcmEventGroup, KEYPRESS_INVALID);
        }
      }
    vTaskDelay(10);
    }
}

/**
 * Creates console task
 **/
void s4743556_tsk_rcmconsole_init() {
    xTaskCreate((void *) &s4743556_rcmconsole_task, (const signed char * const) "RCM Control", CONSOLE_STACK_SIZE, NULL, PRIORITY_CONSOLE, NULL);
}
#endif



