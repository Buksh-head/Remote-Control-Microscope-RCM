/**
 ******************************************************************************
 * @file    s4/main.c
 * @author  Adnaan Buksh - 47435568
 * @date    10042024
 * @brief   
 ******************************************************************************
 */

/* FreeRTOS ----------------------------------------------------------------- */

#include "board.h"
#include "processor_hal.h"
#include "s4743556_sysmon.h"
#include "s4743556_console.h"
#include "s4743556_mfs_pb.h"
#include <stdbool.h>
#include "debug_log.h"


void hardware_init();
void Task1_Task( void );
void Task2_Task( void );
void Task3_Task( void );
void print_task(void);

// Task Priorities (Idle Priority is the lowest priority)
#define TASK1_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK2_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK3_PRIORITY					( tskIDLE_PRIORITY + 2 )

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define TASK1_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK2_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK3_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

// Global variables
TaskHandle_t Task2_Handle;
SemaphoreHandle_t SemaphorePBSig;
QueueHandle_t xQueue;
bool task2_running = false;
int numspaces;


/*
 * Starts all the other tasks, then starts the scheduler. Must not have a Cyclic Executive.
 */
int main( void ) {

	HAL_Init();		//Only HAL_Init() must be called before task creation.

	//
	//Only functions that create tasks must be called in main.
	//

	// Start the task to flash the LED.
	SemaphorePBSig = xSemaphoreCreateBinary();
	hardware_init();
 	xTaskCreate( (void *) &Task1_Task, (const signed char *) "T1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, NULL );
	xTaskCreate( (void *) &Task3_Task, (const signed char *) "T3", TASK3_STACK_SIZE, NULL, TASK3_PRIORITY, NULL );

	/* Start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */

	vTaskStartScheduler();

  return 0;
}

/*
 * Task 1.
 */
void Task1_Task( void ) {

	S4743556_REG_SYSMON_CHAN0_CLR() //Clear LA Channel 0		

	for (;;) {
		taskENTER_CRITICAL();
		S4743556_REG_SYSMON_CHAN0_SET()			//Set LA Channel 0	

		vTaskDelay(3);			//Extra Delay for 3ms	
		S4743556_REG_SYSMON_CHAN0_CLR()			//Clear LA Channel 0	
		taskEXIT_CRITICAL();        
	}
}

/*
 * Task 2.
 */
void Task2_Task( void ) {

	numspaces = 0;

 	S4743556_REG_SYSMON_CHAN1_CLR() //Clear LA Channel 1
	char buffer[31]; 
	xQueue = xQueueCreate(2, sizeof(char)); //Create Queue	

	for (;;) {
		taskENTER_CRITICAL();
        S4743556_REG_SYSMON_CHAN1_SET(); // Set LA Channel 1
        BRD_LEDGreenToggle();		//Random instruction
        
		for (int i = 0; i < numspaces; i++) {
			buffer[i] = ' ';
		}
		buffer[numspaces] = '>';
		buffer[numspaces + 1] = '>';
		buffer[numspaces + 2] = '\0';

		if (numspaces < 28) {
			numspaces += 2;
		} else {
			numspaces = 0;
		}

		xQueueSend(xQueue, (void*) &buffer, (TickType_t) 0); //Send to Queue
		xTaskCreate((void *)&print_task, (const signed char *)"PIRNT", TASK1_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);

        vTaskDelay(3); // Extra Delay for 3ms
        S4743556_REG_SYSMON_CHAN1_CLR(); // Clear LA Channel 1
		taskEXIT_CRITICAL();
    }
}

/*
 * Task3.
 */
void Task3_Task( void ) {

	S4743556_REG_SYSMON_CHAN2_CLR() //Clear LA Channel 2
		
	for (;;) {
		S4743556_REG_SYSMON_CHAN2_SET()			//Set LA Channel 2
		if (SemaphorePBSig != NULL) {	// Check if semaphore exists	
			if (xSemaphoreTake(SemaphorePBSig, 10)) {
				if (Task2_Handle != NULL) {
					vTaskDelete(Task2_Handle);
					Task2_Handle = NULL;
					task2_running = false;
				} else {
					xTaskCreate((void *)&Task2_Task, (const signed char *)"T2_Run", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &Task2_Handle);
					task2_running = true;
				}
			}
		}
		S4743556_REG_SYSMON_CHAN2_CLR()			//Clear LA Channel 2
	}	
}

/*
 * Print Task
 */
void print_task(void) {
	if (numspaces == 0) {
		debug_putc('\r');
		for (int i = 0; i < 31; i++) {
			debug_putc(' ');
		}
	}
	debug_putc('\r');
	char buffer[31] = {0};
	xQueueReceive(xQueue, (void*) &buffer, (TickType_t) 0);
	for (int i = 0; buffer[i] != '\0'; i++) {
		debug_putc(buffer[i]);
	}
	vTaskDelete(NULL);
}

/*
 * Initialise Hardware
 */
void hardware_init(void) {
  taskENTER_CRITICAL();
  
  BRD_LEDInit();				//Initialise Green LED
	BRD_LEDGreenOff();			//Turn off Green LED
  s4743556_sysmon_init();
  s4743556_reg_mfs_pb_init(1);
  BRD_debuguart_init(); 

  taskEXIT_CRITICAL();
}
