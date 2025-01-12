/**
 ******************************************************************************
 * @file    s2/main.c
 * @author  Adnaan Buksh - 47435568
 * @date    01032024
 * @brief   Program that controls the colour and brightness of an RGB LED, using
 *          the pushbutton and trimpot on the MFS, while displaying the
 *          brightness on the LED bar
 ******************************************************************************
 */

#include "board.h"
#include "processor_hal.h"
#include "s4743556_lta1000g.h"
#include "s4743556_mfs_pb.h"
#include "s4743556_mfs_trimpot.h"
#include "s4743556_rgb.h"
#include "debug_log.h"

void hardware_init(void);
void led_increment();
int brightness_control();
void led_bar_increment();
int trimpot_offset();
void brightness_change();

/*
 * Initial Trimpot ADC value
 */
int offset = 0;

/*
 * Pulse width of duty cycle
 */
int pulse_width = 100;

/*
 * Main program
 */
int main(void) {
  HAL_Init();      // Initalise Board
  hardware_init(); // Initalise hardware modules

  s4743556_reg_rgb_colour_set(7);
  s4743556_reg_rgb_brightness_write(pulse_width);
  offset = s4743556_reg_mfs_trimpot_get();

  while (1) { 
    led_increment();
    led_bar_increment();
    brightness_change();
  } 
  return 0;
}

/*
 * Changes the brightness of the RGB Led
 */
void brightness_change() {
  int trimpot_value = s4743556_reg_mfs_trimpot_get();
      //debug_log("Trimpot: %d\r\n", trimpot_value);
      //debug_log("Offset: %d\r\n", offset);
      if ( trimpot_value > (offset + 250)) {
        if (pulse_width == 50) {
          pulse_width = 100;
          s4743556_reg_rgb_brightness_write(pulse_width);
        } else {
          s4743556_reg_rgb_brightness_write(pulse_width - 5);
          pulse_width = pulse_width - 5;
        }
        offset = trimpot_value;
      } 
      if (trimpot_value < (offset - 250)) {
        if (pulse_width == 100) {
          pulse_width = 50;
          s4743556_reg_rgb_brightness_write(pulse_width);
        } else {
          s4743556_reg_rgb_brightness_write(pulse_width + 5);
          pulse_width = pulse_width + 5;
        }
        offset = trimpot_value;
      }
}

/*
 * Increments led bar
 */
void led_bar_increment() {
  for (int i = 0; i < 10; i++) {
    if (i < (s4743556_reg_rgb_brightness_read() / 10)) {
      lta1000g_seg_set(i,'1');
    } else {
      lta1000g_seg_set(i,'0');
    }
  }
  for (int i = 9; i > (s4743556_reg_rgb_brightness_read() / 10); i--) {
    lta1000g_seg_set(i,'0');
  }
  
}

/*
 * Changes color of led
 */
void led_increment() {
  int colourList [8] = {7,3,5,6,4,2,1,0};
  s4743556_reg_rgb_colour_set(colourList[s4743556_reg_mfs_pb_press_get(1) % 8]);
  
}

/*
 * Initialise Hardware
 */
void hardware_init(void) {
  s4743556_lta1000g_init();
  s4743556_reg_mfs_pb_init(1);
  s4743556_reg_mfs_trimpot_init();
  s4743556_reg_rgb_init();
  BRD_debuguart_init(); 
}