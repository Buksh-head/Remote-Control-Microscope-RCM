/**
 ******************************************************************************
 * @file    s3/main.c
 * @author  Adnaan Buksh - 47435568
 * @date    01032024
 * @brief   Program that receives input from the console and encodes/decodes
 *          the input using Hamming 7 / 4. While displaying the decoded/encoded
 *          byte on the LED bar. Board LEDs show 1 bit and 2 bit errors.
 ******************************************************************************
 */

#include "board.h"
#include "processor_hal.h"
#include "s4743556_mfs_led.h"
#include "s4743556_hamming.h"
#include "s4743556_console.h"
#include "s4743556_lta1000g.h"
#include "s4743556_mfs_pb.h"
#include <stdbool.h>
#include "debug_log.h"

#define TIMER_COUNTER_FREQ  50000           //Frequency (in Hz)
#define TIMER_1SECOND_PERIOD    2500       //Period for 50ms (in count ticks)

typedef enum {
    IDLE_STATE,
    ENCODE_STATE,
    DECODE_STATE
} FSM_State;

// Global variables
static FSM_State state; // Current state of the FSM
static char recvChar; // Character received from input
uint8_t upperByteMode = 0; // Display upper byte
static unsigned short receivedByte; // Byte received from input
static char prevChar = '\0'; // Previous character received
uint8_t ledStatus = 0; // Status of the LED (on/off)
int encode_num = 0; // Number to be encoded
int decode_num = 0; // Number to be decoded

void tui_callback(void);
void hardware_init(void);
void handle_input(void);
void toggle_led(void);
void toggle_ledbar(void);
void check_state(void);

int main(void) {
  HAL_Init();      // Initalise Board
  hardware_init(); // Initalise hardware modules
  state = IDLE_STATE;
  S4743556_REG_MFS_LED_D3_OFF();
  S4743556_REG_MFS_LED_D1_OFF();
  S4743556_REG_MFS_LED_D2_OFF();
  S4743556_REG_MFS_LED_D4_OFF();
  while (1) { 
    
  } 
  return 0;
}

/*
 * Handle input based on the current FSM state
 */
void handle_input(void) {
  toggle_led();
  check_state();
  switch (state) {
    case IDLE_STATE:
      debug_log("Char %c\n\r", recvChar);
      prevChar = recvChar;
      break;

    case ENCODE_STATE:
      if (s4743556_lib_console_ascii2hex(recvChar) != -1 && prevChar != '\0' &&
        s4743556_lib_console_ascii2hex(prevChar) != -1) {
        char encodeChar[3];
        encodeChar[0] = prevChar;
        encodeChar[1] = recvChar;
        encode_num =  s4743556_lib_console_ascii2hex(encodeChar[0]) << 4 |
          s4743556_lib_console_ascii2hex(encodeChar[1]);
        unsigned short encodedByte = s4743556_lib_hamming_byte_encode(encode_num);
        debug_log("Encode %02X to %04X\n\r", encode_num, encodedByte);
        receivedByte = encodedByte;
        prevChar = '\0';          
      } else {
          prevChar = recvChar;
      }
      break;

    case DECODE_STATE:
      if (s4743556_lib_console_ascii2hex(recvChar) != -1 && prevChar != '\0' &&
        s4743556_lib_console_ascii2hex(prevChar) != -1) {
        char decodeChar[3];
        decodeChar[0] = prevChar;
        decodeChar[1] = recvChar;
        decode_num =  s4743556_lib_console_ascii2hex(decodeChar[0]) << 4 |
          s4743556_lib_console_ascii2hex(decodeChar[1]);
        unsigned char decodedByte = s4743556_lib_hamming_byte_decode(decode_num);
        
        uint8_t d0, d1, d2, d3, h0, h1, h2, s2, s1, s0;
        // Extract Hamming bits and Data bits.
        h0 = !!(decode_num & (1 << 1));
        h1 = !!(decode_num & (1 << 2));
        h2 = !!(decode_num & (1 << 3));
        d0 = !!(decode_num & (1 << 4));
        d1 = !!(decode_num & (1 << 5));
        d2 = !!(decode_num & (1 << 6));
        d3 = !!(decode_num & (1 << 7));

        // Calculate the Syndrome.
        s0 = h0 ^ d1 ^ d2 ^ d3;
        s1 = h1 ^ d0 ^ d2 ^ d3;
        s2 = h2 ^ d0 ^ d1 ^ d3;

        // Check for errors based on the syndrome and parity
        unsigned char parityError = s4743556_lib_hamming_parity_error(decode_num);
        unsigned char syndrome = (s0 << 0) + (s1 << 1) + (s2 << 2);

        if (syndrome == 0) {
          // No errors occurred
          if (parityError) {
            // Parity error
            S4743556_REG_MFS_LED_D1_ON();
            S4743556_REG_MFS_LED_D2_OFF();
          } else {
            S4743556_REG_MFS_LED_D1_OFF();
            S4743556_REG_MFS_LED_D2_OFF();
          }
        } else if (!parityError) {
          // Two-bit error
          S4743556_REG_MFS_LED_D1_OFF();
          S4743556_REG_MFS_LED_D2_ON();
        } else {
          // Single-bit error
          S4743556_REG_MFS_LED_D1_ON();
          S4743556_REG_MFS_LED_D2_OFF();
        }

        debug_log("Decode %02X to %02X\n\r", decode_num, decodedByte);
        receivedByte = decodedByte;
        prevChar = '\0';          
      } else {
          prevChar = recvChar;
      }
      break;
  }
}

/*
 * Toggle the LED bar based from lower to higher byte when pushbutton is pressed
 */
void toggle_ledbar(void) {
  if (s4743556_reg_mfs_pb_press_get(1)) {
      if (upperByteMode) {
        upperByteMode = 0;
      } else {
        upperByteMode = 1;
      }
      s4743556_reg_mfs_pb_press_reset(1);
    }
    if (upperByteMode) {
      s4743556_reg_lta1000g_write((receivedByte & 0xFF00) >> 8); // Display upper byte
      S4743556_REG_MFS_LED_D3_ON();
    } else {
      s4743556_reg_lta1000g_write(receivedByte & 0x00FF); // Display lower byte
      S4743556_REG_MFS_LED_D3_OFF(); // Turn off D3 LED to indicate lower byte mode
    }
}

/*
 * Toggle the LED D4 after every console input
 */
void toggle_led(void) {
  ledStatus ^= 1;
  if (ledStatus) {
    S4743556_REG_MFS_LED_D4_ON();
  } else {
    S4743556_REG_MFS_LED_D4_OFF();
  }
}

/*
 * Check and changes the state of the fsm and turns off bit error LEDs
 */
void check_state(void) {
  if (recvChar == '#' && prevChar == '#') {
    state = ENCODE_STATE;
    S4743556_REG_MFS_LED_D1_OFF();
    S4743556_REG_MFS_LED_D2_OFF();
  } else if (recvChar == '*' && prevChar == '*') {
    state = DECODE_STATE;
  } else if (recvChar == '!' && prevChar == '!') {
    state = IDLE_STATE;
    S4743556_REG_MFS_LED_D1_OFF();
    S4743556_REG_MFS_LED_D2_OFF();
  } 
}

/*
 * Timer update interrupt callback
 */
void tui_callback(void) {
  if ((recvChar = BRD_debuguart_getc(0)) != '\0') {
    handle_input();
  }
  toggle_ledbar();
}

/*
 * Interrupt handler for Timer 2 Interrupt Handler
 */ 
void TIM2_IRQHandler(void) {
	//Check and clear overflow flag.
	if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF) {	// Check if overflow has taken place 
		TIM2->SR &= ~TIM_SR_UIF; 					// Clear the UIF Flag
		tui_callback();   // Callback for timer update interrupt
	}
}

/*
 * Initialise Hardware
 */
void hardware_init(void) {

  BRD_LEDInit();

  // Timer 2 clock enable
	__TIM2_CLK_ENABLE();

	// Compute the prescaler value to set the timer counting frequency to 50kHz
	// SystemCoreClock is the system clock frequency
	TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM2->CR1 |= TIM_CR1_DIR; 

	TIM2->CNT = TIMER_1SECOND_PERIOD;	//Load counter register with 1s period value
	TIM2->ARR = TIMER_1SECOND_PERIOD;	//Load auto reload register with 1s period value

	// Enable related interrupts
	TIM2->DIER |= TIM_DIER_UIE;			// Enable update interrupt to occur on timer counter overflow or underflow.
	
	//Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
	HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->CR1 |= TIM_CR1_CEN;				// Enable the counter

  s4743556_reg_mfs_led_init();
  s4743556_reg_mfs_pb_init(1);
  s4743556_lta1000g_init();
  BRD_debuguart_init(); 
}