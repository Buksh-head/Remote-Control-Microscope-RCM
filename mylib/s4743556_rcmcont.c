 /** 
 **************************************************************
 * @file mylib/s4743556_rcmcont.c
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief RCM control is used to manipulate the RCM’s location and control the 
 * zoom and rotation.
 * RCM control must process the console inputs and control the radio 
 * transmitter and console display, As an FSM.
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_rcmcontrol_init() - creates control task
 ***************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "s4743556_rcmcont.h"
#include "s4743556_mfs_led.h"
#include "s4743556_console.h"
#include "s4743556_rcmdisplay.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

typedef enum {
    IDLE_STATE,
    PROCESS_STATE,
    DISPLAY_STATE,
    TRANSMIT_STATE,
} RCM_State;

RCM_State rcmState = IDLE_STATE;

EventGroupHandle_t s4743556rcmEventGroup;
QueueHandle_t s4743556radioTxQueue;
QueueHandle_t s4743556rcmQueuePosition;

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

uint8_t ledOn = 0;
uint8_t ledOff = 1;

int moveX = 0;
int moveY = 0;
int moveZ = 0;
int zoom = 0;
int rotate = 0;

int rcmX = 0;
int rcmY = 0;
int rcmZ = 0;
int rcmZoom = ZOOM_MIN; // 1 to 9
int rcmRotate = ROTATE_MIN; // 0 to 180

void zero_pad(char* str, int target_len);
void toggle_console_led();
void toggle_packet_led();

/**
 * initialise RCM
 */
void s4743556_rcmcont_init() { 
    s4743556_reg_mfs_led_init();
    S4743556_REG_MFS_LED_D3_OFF();
    S4743556_REG_MFS_LED_D4_OFF();

    s4743556rcmEventGroup = xEventGroupCreate();
    s4743556radioTxQueue = xQueueCreate(10, sizeof(uint8_t[BUFFER]));
}

/**
 * Recieves keypresses and manipulates the RCM's location, zoom and rotation
 * and all LED's. Send the data to the display and radio transmitter
 */
void s4743556_rcmcont_task() {

    taskENTER_CRITICAL();
    s4743556_rcmcont_init();
    taskEXIT_CRITICAL();

    S4743556_REG_MFS_LED_D1_OFF(); 
    S4743556_REG_MFS_LED_D2_OFF(); 

    EventBits_t uxBits;
    rcmPos rcmPos;

    int nextRCMState = IDLE_STATE;
    int type;
    rcmPos.grid = false;
    bool origin = false;
    
    for(;;) {
        switch (rcmState) {
            case IDLE_STATE:
                uxBits = xEventGroupWaitBits(s4743556rcmEventGroup, KEYPRESS_EVENT, pdTRUE, pdFALSE, 10);
                if ((uxBits & KEYPRESS_Q) != 0) {
                    moveX = SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_W) != 0) {
                    moveX = -SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_E) != 0) {
                    moveY = SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_R) != 0) {
                    moveY = -SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_T) != 0) {
                    moveZ = SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_Y) != 0) {
                    moveZ = -SMALL_MOVE;
                } else if ((uxBits & KEYPRESS_A) != 0) {
                    moveX = MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_S) != 0) {
                    moveX = -MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_D) != 0) {
                    moveY = MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_F) != 0) {
                    moveY = -MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_G) != 0) {
                    moveZ = MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_H) != 0) {
                    moveZ = -MEDIUM_MOVE;
                } else if ((uxBits & KEYPRESS_Z) != 0) {
                    moveX = LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_X) != 0) {
                    moveX = -LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_C) != 0) {
                    moveY = LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_V) != 0) {
                    moveY = -LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_B) != 0) {
                    moveZ = LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_N) != 0) {
                    moveZ = -LARGE_MOVE;
                } else if ((uxBits & KEYPRESS_1) != 0) {
                    zoom = ZOOM_IN;
                } else if ((uxBits & KEYPRESS_2) != 0) {
                    zoom = ZOOM_OUT;
                } else if ((uxBits & KEYPRESS_3) != 0) {
                    rotate = ROTATE_LEFT;
                } else if ((uxBits & KEYPRESS_4) != 0) {
                    rotate = ROTATE_RIGHT;
                } 
                nextRCMState = PROCESS_STATE;
                if ((uxBits & KEYPRESS_5) != 0) {
                    rcmX = 0;
                    rcmY = 0;
                    rcmZ = 0;
                    rcmRotate = ROTATE_MIN;
                    rcmZoom = ZOOM_MIN;
                    type = MOVE_CODE;
                    rcmPos.grid = true;
                    origin = true;
                    nextRCMState = DISPLAY_STATE;
                } 
                if ((uxBits & KEYPRESS_INVALID) != 0) {
                    nextRCMState = IDLE_STATE;   
                } 
                if (uxBits != 0) {
                    toggle_console_led();
                } else {
                    nextRCMState = IDLE_STATE;
                }
                break;
            case PROCESS_STATE:
                if (rotate != 0) {
                    type = ROTATE_CODE;
                } else if (zoom != 0) {
                    type = ZOOM_CODE;
                } else {
                    type = MOVE_CODE;
                }
                rcmX += moveX;
                rcmY += moveY;
                rcmZ += moveZ;
                rcmZoom += zoom;
                rcmRotate += rotate;

                if (rcmX < 0) {
                    rcmX = 0;
                } else if (rcmX > X_MAX) {
                    rcmX = X_MAX;
                }
                if (rcmY < 0) {
                    rcmY = 0;
                } else if (rcmY > Y_MAX) {
                    rcmY = Y_MAX;
                }
                if (rcmZ <= 0) {
                    rcmZ = 0;
                } else if (rcmZ > Z_MAX) {
                    rcmZ = Z_MAX;
                }                    
                if (rcmZoom < ZOOM_MIN) {
                    rcmZoom = ZOOM_MIN;
                } else if (rcmZoom > ZOOM_MAX) {
                    rcmZoom = ZOOM_MAX;
                } 
                if (rcmRotate < ROTATE_MIN) {
                    rcmRotate = ROTATE_MIN;
                } else if (rcmRotate > ROTATE_MAX) {
                    rcmRotate = ROTATE_MAX;
                }

                if (moveX != 0) {
                    rcmPos.xMoved = true;
                    rcmPos.yMoved = false;
                    rcmPos.grid = true;
                } else if (moveY != 0) {
                    rcmPos.yMoved = true;
                    rcmPos.xMoved = false;
                    rcmPos.grid = true;
                } else {
                    rcmPos.xMoved = false;
                    rcmPos.yMoved = false;
                }

                nextRCMState = DISPLAY_STATE;
                rcmPos.movedZ = moveZ;

                // Zero all movement values
                moveX = 0;
                moveY = 0;
                moveZ = 0;
                zoom = 0;
                rotate = 0;
                break;
            case DISPLAY_STATE:
                if (origin) {
                    rcmPos.movedZ = -Z_MAX;
                    rcmPos.xMoved = false;
                    rcmPos.yMoved = false;
                }
                rcmPos.x = rcmX;
                rcmPos.y = rcmY;
                rcmPos.Zvalue = rcmZ;
                rcmPos.rotateValue = rcmRotate;
                xQueueSend(s4743556rcmQueuePosition, &rcmPos, 10);
                rcmPos.grid = false;
                nextRCMState = TRANSMIT_STATE;
                break;
            case TRANSMIT_STATE:    
                if (type == MOVE_CODE) {
                    char x_str[4]; // For three-digit numbers + null terminator
                    char y_str[4];
                    char z_str[3]; 
                    char payload[PAYLOAD_BUF];
                    uint8_t packet[BUFFER] = {type, 0x47, 0x43, 0x55, 0x68};
                    snprintf(x_str, sizeof(x_str), "%03d", rcmX); // Ensures 3 digits with leading zeros
                    snprintf(y_str, sizeof(y_str), "%03d", rcmY);
                    snprintf(z_str, sizeof(z_str), "%02d", rcmZ);
                    snprintf(payload, PAYLOAD_BUF, "XYZ%s%s%s", x_str, y_str, z_str);
                    zero_pad(payload, PAYLOAD_BUF - 1);  
                    strncpy((char*)&packet[5], payload, PAYLOAD_BUF - 1);
                    xQueueSend(s4743556radioTxQueue, &packet, 10);
                    toggle_packet_led();
                } else if (type == ROTATE_CODE) {
                    char rot_str[4]; 
                    char payload[PAYLOAD_BUF];
                    uint8_t packet[BUFFER] = {type, 0x47, 0x43, 0x55, 0x68};
                    snprintf(rot_str, sizeof(rot_str), "%03d", rcmRotate);
                    snprintf(payload, PAYLOAD_BUF, "ROT%s", rot_str);
                    zero_pad(payload, PAYLOAD_BUF - 1);  
                    strncpy((char*)&packet[5], payload, PAYLOAD_BUF - 1);
                    xQueueSend(s4743556radioTxQueue, &packet, 10);
                    toggle_packet_led();
                } else if (type == ZOOM_CODE) {
                    char zoom_str[2]; 
                    char payload[PAYLOAD_BUF];
                    uint8_t packet[BUFFER] = {type, 0x47, 0x43, 0x55, 0x68};
                    snprintf(zoom_str, sizeof(zoom_str), "%01d", rcmZoom);
                    snprintf(payload, PAYLOAD_BUF, "ZOOM%s", zoom_str);
                    zero_pad(payload, PAYLOAD_BUF - 1);
                    strncpy((char*)&packet[5], payload, PAYLOAD_BUF - 1);
                    xQueueSend(s4743556radioTxQueue, &packet, 10);
                    toggle_packet_led();
                }
                nextRCMState = IDLE_STATE;
                if (origin) {
                    if (type == ROTATE_CODE) {
                        type = ZOOM_CODE;
                        origin = false;
                    } else {
                        type = ROTATE_CODE;
                    }
                    nextRCMState = DISPLAY_STATE;
                }
                break;
        }
        rcmState = nextRCMState;
        vTaskDelay(3);
    }
}

/**
 * Pads a string with zeros to a target length
 */
void zero_pad(char* str, int target_len) {
    // Ensure the string has the right length by padding with zeros
    int len = strlen(str);
    for (int i = len; i < target_len; i++) {
        str[i] = '0';
    }
    str[target_len] = '\0';
}

/**
 * Toggles the D2 LED
 */
void toggle_console_led() {
    ledOn = !ledOn;
    if (ledOn) {
        S4743556_REG_MFS_LED_D2_ON();
    } else {
        S4743556_REG_MFS_LED_D2_OFF();
    }
}

/**
 * Toggles the D1 LED
 */
void toggle_packet_led() {
    ledOff = !ledOff;
    if (ledOff) {
        S4743556_REG_MFS_LED_D1_ON();
    } else {
        S4743556_REG_MFS_LED_D1_OFF();
    }
}

/**
 * Creates control task
 */
void s4743556_tsk_rcmcontrol_init() {
    xTaskCreate((void *) &s4743556_rcmcont_task, "Control Task", CONTROL_STACK_SIZE, NULL, PRIORITY_CONTROL, NULL);
}

