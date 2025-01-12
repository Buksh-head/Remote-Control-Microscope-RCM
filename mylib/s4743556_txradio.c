 /** 
 **************************************************************
 * @file mylib/s4743556_txradio.c
 * @author Adnaan Buksh - 47435568
 * @date 22042024
 * @brief 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_tsk_txradio_init() - creates txradio task
 ***************************************************************
 */
//#define DEBUG   
#ifdef DEBUG
#include "debug_log.h"
#endif

#include <stdint.h>
#include "s4743556_txradio.h"
#include "nrf24l01plus.h"
#include "s4743556_hamming.h"
#include "s4743556_brd_pb.h"
#include "s4743556_mfs_led.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

typedef enum {
    INIT_STATE,
    IDLE_STATE,
    TX_STATE,
} Radio_State;

Radio_State state = INIT_STATE;
SemaphoreHandle_t s4743556brdSemaphorePBSig;
uint8_t pbPressed = 0;

/**
 * Initialises peripherals for the radio task
 */
void s4743556_txradio_init() {
    nrf24l01plus_init(); // Initialise NRF24L01plus module
    BRD_LEDInit();
    s4743556_reg_brd_pb_init();
}

/**
 * Receives data from the radio queue, encodes and transmits 
 */
void s4743556_txradio_task() {

    taskENTER_CRITICAL();
    s4743556_txradio_init();
    taskEXIT_CRITICAL();

    uint8_t queuedData[BUFFER_SIZE];
    uint8_t data[BUFFER_SIZE] = {0x00};
    uint8_t dataEncoded[BUFFER_SIZE * 2] = {0x00};
    
    s4743556radioTxQueue = xQueueCreate(10, sizeof(queuedData));
    s4743556brdSemaphorePBSig = xSemaphoreCreateBinary();
    
    for(;;) {
        int nextState;
        switch (state) {
            case INIT_STATE:
                if (s4743556brdSemaphorePBSig != NULL) {	
                    if (xSemaphoreTake(s4743556brdSemaphorePBSig, 10) == pdTRUE) {
                        data[0] = 0x20;
                        data[1] = 0x47;
                        data[2] = 0x43;
                        data[3] = 0x55;
                        data[4] = 0x68;
                        data[5] = 'J';
                        data[6] = 'O';
                        data[7] = 'I';
                        data[8] = 'N';
                        #ifdef DEBUG
                        BRD_LEDRedToggle();
                        #endif
                        S4743556_REG_MFS_LED_D1_ON();
                        pbPressed = 1;
                        nextState = TX_STATE;
                    }else{
                        nextState = INIT_STATE;
                        pbPressed = 0;
                    }
                }
                break;
            case IDLE_STATE:
                if (s4743556radioTxQueue != NULL) {
                    if (xQueueReceive(s4743556radioTxQueue, &queuedData, 10) == pdTRUE) {
                        for (int i = 0; i < BUFFER_SIZE; i++) {
                            data[i] = queuedData[i];
                        }
                        nextState = TX_STATE;
                    }
                }
                else {
                    nextState = IDLE_STATE;
                }  
                break;
            case TX_STATE:
                //encode data
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    #ifdef DEBUG
                    debug_putc(data[i]);
                    #endif
                    uint16_t encodedByte = s4743556_lib_hamming_byte_encode(data[i]);
                    dataEncoded[i*2] = encodedByte & 0xFF;
                    dataEncoded[i*2 + 1] = (encodedByte >> 8) & 0xFF;
                }
                nrf24l01plus_send(dataEncoded);
                nextState = IDLE_STATE;
                break;
        }
        state = nextState;
        vTaskDelay(10);			//Extra Delay for 10ms
    } 
}

/**
 * Creates txradio task
 */
void s4743556_tsk_txradio_init() {
    xTaskCreate((void *) &s4743556_txradio_task, "Radio Task", RADIO_STACK_SIZE, NULL, PRIORITY_RADIO, NULL);
}