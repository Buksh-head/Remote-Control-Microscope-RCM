/**
 ******************************************************************************
 * @file    s1/main.c
 * @author  Adnaan Buksh - 47435568
 * @date    21022024
 * @brief   Program that iterates through the fibonacci sequence on and led
 *          while haveing the functionality to invert 5 uppper bits, while
 *          simultaneously having a pin change state from high to low.
 ******************************************************************************
 */

#include "board.h"
#include "processor_hal.h"
#include "s4743556_lta1000g.h"
#include "s4743556_mfs_pb.h"

void hardware_init(void);
void heartbeat_init(void);
void heartbeat_toggle(unsigned int prevTick, unsigned int duration);
void fibonacci_toggle(unsigned int prevTick);
void invert_upper_bits();
unsigned int fibonacci_sequence(int n);
void led_top_toggle();

/*
 * Fibonacci sequence counter
 */
unsigned int counter = 0;

/*
 * State of button
 */
static int buttonPressed = 0;

/*
 * Main program
 */
int main(void) {
  unsigned int prevTick;
  prevTick = 0;

  HAL_Init();      // Initalise Board
  hardware_init(); // Initalise hardware modules

  while (1) { 
    led_top_toggle();
    if ((HAL_GetTick() - prevTick) >= 1000) {
      fibonacci_toggle(prevTick);
      heartbeat_toggle(prevTick, 1000);
      prevTick = HAL_GetTick();
    }
  }
  return 0;
}

/*
 * Checks if button was pressed to change state
 */
void led_top_toggle(){
  if (s4743556_reg_mfs_pb_press_get(1)) {
      if (buttonPressed) {
        buttonPressed = 0;
      } else {
        buttonPressed = 1;
      }
      s4743556_reg_mfs_pb_press_reset(1);
    }
  if (buttonPressed) {
    invert_upper_bits();
  } else {
    s4743556_reg_lta1000g_write(fibonacci_sequence(counter));
  }
}

/*
 * Fibonacci sequence increment
 */
void fibonacci_toggle(unsigned int prevTick) {
  if ((HAL_GetTick() - prevTick) >= 1000) {
    counter++;
    if (counter > 15) { // Reset counter at F15
      counter = 0;
    }
  }
}

/*
 * Inverting top 5 bits of the number
 */
void invert_upper_bits() {
  unsigned short upperBits = ((fibonacci_sequence(counter) >> 5) & 0x001F);
  unsigned short invertedUpperBits = ~upperBits;
  unsigned short modifiedValue = fibonacci_sequence(counter);
  modifiedValue &= 0x001F;
  modifiedValue |= invertedUpperBits << 5;
  s4743556_reg_lta1000g_write(modifiedValue);
}

/*
 * Calculate the nth Fibonacci number
 */
unsigned int fibonacci_sequence(int n) {
  if (n <= 1) {
    return n;
  } else if (n >= 610) {
    return 0;
  } else {
    return fibonacci_sequence(n - 1) + fibonacci_sequence(n - 2);
  }
}

/*
 * HeartBeat port toggle and led increment
 */
void heartbeat_toggle(unsigned int prevTick, unsigned int duration) {
  if ((HAL_GetTick() - prevTick) >= duration) {
    GPIOD->ODR ^= (1 << 10);
  }
}

/*
 * HeartBeat port set up
 */
void heartbeat_init() {
  __GPIOD_CLK_ENABLE();
  GPIOD->MODER |= (0x01 << (10 * 2)); // Set as output
  GPIOD->OSPEEDR |= (GPIO_SPEED_FREQ_LOW << (10 * 2));
  GPIOD->PUPDR &= ~(0x03 << (10 * 2));
}

/*
 * Initialise Hardware
 */
void hardware_init(void) {
  s4743556_lta1000g_init();
  heartbeat_init();
  s4743556_reg_mfs_pb_init(1);
}