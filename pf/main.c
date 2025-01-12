/**
 ******************************************************************************
 * @file    pf/main.c
 * @author  Adnaan Buksh - 47435568
 * @date    22042024
 * @brief   
 ******************************************************************************
 */

#include "board.h"
#include "processor_hal.h"

void s4743556_tsk_txradio_init();
void s4743556_tsk_rcmconsole_init();
void s4743556_tsk_rcmcontrol_init();
void s4743556_tsk_rcmdisplay_init();
void vTaskStartScheduler( void );


/*
 * Starts all the other tasks, then starts the scheduler. Must not have a Cyclic Executive.
 */
int main( void ) {
	HAL_Init();		//Only HAL_Init() must be called before task creation.

	//
	//Only functions that create tasks must be called in main.
	//
	s4743556_tsk_txradio_init();
	s4743556_tsk_rcmconsole_init();
	s4743556_tsk_rcmcontrol_init();
	s4743556_tsk_rcmdisplay_init();
	
	vTaskStartScheduler();
	/* Start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	return 0;
}