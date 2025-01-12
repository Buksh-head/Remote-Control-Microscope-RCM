/** 
 **************************************************************
 * @file mylib/s4743556_lta1000g.c
 * @author Adnaan Buksh - 47435568
 * @date 21022024
 * @brief LTA1000G LED Bar Display Mylib Register Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_lta1000g_init() - intialise mylib template driver
 * s4743556_lta1000g_set() - set GPIO pin high or low
 * s4743556_reg_lta1000g_write() - Write the LED Bar segments high or low
 * s4743556_reg_lta1000g_invert() - Inverts the LED Bar segments
 *************************************************************** 
 */

#include "s4743556_lta1000g.h"
#include <stm32f429xx.h>

/*
 * Initialise LED Bar GPIO pins as outputs
 */
void s4743556_lta1000g_init () {

    // Enable the GPIO Clock
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();

    uint16_t g_pins[] = {5, 6, 8, 10, 11, 13, 15};

    //Initialise as outputs.
    GPIOE->MODER &= ~(0x03 << (0 * 2));    // Clear PE0
    GPIOE->MODER |= (0x01 << (0 * 2));     // Set for push-pull output
    GPIOE->OSPEEDR &= ~(0x03<<(0 * 2));
	GPIOE->OSPEEDR |=   0x02<<(0 * 2);     // Set for Fast speed
    GPIOE->OTYPER &= ~(0x01 << 0);         //Clear Bit for Push/Pull Output

    GPIOE->MODER &= ~(0x03 << (6 * 2));    // PE6
    GPIOE->MODER |= (0x01 << (6 * 2));
    GPIOE->OSPEEDR &= ~(0x03<<(6 * 2));
	GPIOE->OSPEEDR |=   0x02<<(6 * 2);     
    GPIOE->OTYPER &= ~(0x01 << 6); 

    GPIOF->MODER &= ~(0x03 << (11 * 2));   // PF11
    GPIOF->MODER |= (0x01 << (11 * 2));
    GPIOF->OSPEEDR &= ~(0x03<<(11 * 2));
	GPIOF->OSPEEDR |=   0x02<<(11 * 2);     
    GPIOF->OTYPER &= ~(0x01 << 11);       

    for (unsigned int i = 0; i < sizeof(g_pins) / sizeof(g_pins[0]); i++) {
        GPIOG->MODER &= ~(0x03 << (g_pins[i] * 2));
        GPIOG->MODER |= (0x01 << (g_pins[i] * 2));
        GPIOG->OSPEEDR &= ~(0x03 << (g_pins[i] * 2));
        GPIOG->OSPEEDR |= 0x02 << (g_pins[i] * 2);     
        GPIOG->OTYPER &= ~(0x01 << g_pins[i]);
    }

}

/*
 * Set the value (0 or 1) for the corresponding LED Bar segment GPIOG pin.
 */
void seg_set_helper (unsigned char value, int bit) {
    if (value == '0') {
        GPIOG->ODR &= ~(0X01 << bit);
    } else if (value == '1') {
        GPIOG->ODR |= (0X01 << bit);
    }
}

/*
 * Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin.
 */
void lta1000g_seg_set (int segment, unsigned char value) {
    switch (segment)
    {
    case 0:
        if (value == '0') {
            GPIOE->ODR &= ~(0X01 << 6);
        } else if (value == '1') {
            GPIOE->ODR |= (0X01 << 6);
        }
        return;
    
    case 1:
        seg_set_helper(value, 15);
        return;
      
    case 2:
        seg_set_helper(value, 10);
        return;

    case 3:
        seg_set_helper(value, 13);
        return;

    case 4:      
        seg_set_helper(value, 11);
        return;

    case 5:
        if (value == '0') {
            GPIOF->ODR &= ~(0X01 << 11);
        } else if (value == '1') {
            GPIOF->ODR |= (0X01 << 11);
        }
        return;

    case 6:
        if (value == '0') {
            GPIOE->ODR &= ~(0X01 << 0);
        } else if (value == '1') {
            GPIOE->ODR |= (0X01 << 0);
        }
        return;

    case 7:
        seg_set_helper(value, 8);
        return;

    case 8:
        seg_set_helper(value, 5);
        return;
      return;

    case 9:
        seg_set_helper(value, 6);
        return;
      return;
    }
}

/*
 * Write the LED Bar segments high or low, 
 * i.e. LED Bar 0 will be ‘low’ if bit 0 of value is 0,
 * LED Bar 1 will be ‘high’ if bit 1 of value is 1.
 */
void s4743556_reg_lta1000g_write (unsigned short value) {
    for (int i = 0; i < 10; i++) {
        // Check each bit of the value and call lta1000g_seg_set() accordingly
        lta1000g_seg_set(i, (value & (1 << i)) ? '1' : '0');
    }
}

/*
 * Write the LED Bar segments high or low, depending on the value and mask.
 * If a mask bit is set to 1, then the corresponding value bit must be inverted
 * and then used to set the LED Bar segment. 
 * e.g. value=0x0003, mask=0xFFFF, will turn on LED segments 9 down to 2 
 * and turn off LED segments 1 down to 0. 
 * i.e. LED Bar 0 will be ‘low’ if bit 0 of value is 0, 
 * LED Bar 1 will be ‘high’ if bit 1 of value is 1.
 */
void s4743556_reg_lta1000g_invert (unsigned short value, unsigned short mask) {
    unsigned short output = value ^ mask;
    for (int i = 0; i < 10; i++) {
        // Check each bit of the value and call lta1000g_seg_set() accordingly
        lta1000g_seg_set(i, (value & (1 << i)) ? '0' : '1');
    }
}