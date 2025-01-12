 /** 
 **************************************************************
 * @file mylib/s4743556_ssd.h
 * @author Adnaan Buksh - 47435568
 * @date 10052024
 * @brief Seven Segment Display Mylib Register Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_ssd_init() - intialise mylib driver
 * s4743556_ssd_set() - set the 7 segment display to a number
 * s4743556_ssd_clear() - clear the 7 segment display 
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_ssd.h"

void digital_write(int pin, int value);

/**
* Initialise pins used for the seven segment display.
*/
void s4743556_ssd_init () {
    __GPIOF_CLK_ENABLE();

    uint16_t f_pins[] = {DATA_PIN, CLK_PIN, LATCH_PIN};
    for (unsigned int i = 0; i < sizeof(f_pins) / sizeof(f_pins[0]); i++) {
        GPIOF->MODER &= ~(0x03 << (f_pins[i] * 2));
        GPIOF->MODER |= (0x01 << (f_pins[i] * 2));
        GPIOF->OSPEEDR &= ~(0x03 << (f_pins[i] * 2));
        GPIOF->OSPEEDR |= 0x02 << (f_pins[i] * 2);     
        GPIOF->OTYPER &= ~(0x01 << f_pins[i]);
    }
}

/**
* Convert a number to a 7 segment equivelent number.
*/
int convert_num(int n) {
    int m;
    switch(n) {
        case 0: m = 0xC0; break;
        case 1: m = 0xF9; break;
        case 2: m = 0xA4; break;
        case 3: m = 0xB0; break;
        case 4: m = 0x99; break;
        case 5: m = 0x92; break;
        case 6: m = 0x82; break;
        case 7: m = 0xF8; break;
        case 8: m = 0x80; break;
        case 9: m = 0x90;
    }
    return m;
}

/**
 * Displays the number on on of the digits.
*/
void set_num(int dataByte)
{
    for(int i=0; i<8; i++)
    {
        if(dataByte & 0x80){
            digital_write(DATA_PIN, 1);
        } else {
            digital_write(DATA_PIN, 0);
        } 
        dataByte = dataByte << 1;

        digital_write(CLK_PIN, 0); 
        digital_write(CLK_PIN, 1);
    } 
}

/**
 * Write to a pin to high or low.
*/
void digital_write(int pin, int value) {
    if (value == 1) {
        GPIOF->ODR |= (1 << pin);
    } else {
        GPIOF->ODR &= ~(1 << pin);
    }
}

/**
 * Clear the 7 segment display.
*/
void s4743556_ssd_clear() {
    for(int i=0; i<=3; i++)
    {
        set_num(0xFF);
        set_num(0x01 << i);
        digital_write(LATCH_PIN, 0); digital_write(LATCH_PIN, 1);
    }
}

/**
 * Set a digit of the display to a number.
*/
void s4743556_ssd_set(int digit, int num) {
    int digit1;
    switch (digit) {
        case 1: digit1 = 8; break;
        case 2: digit1 = 4; break;
        case 3: digit1 = 2; break;
        case 4: digit1 = 1;
    }
    int m = convert_num(num);
    set_num(m);
    set_num(digit1);
    digital_write(LATCH_PIN, 0); 
    digital_write(LATCH_PIN, 1);
}
