 /** 
 **************************************************************
 * @file mylib/s4743556_rgb.c
 * @author Adnaan Buksh - 47435568
 * @date 01032024
 * @brief RGB Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_rgb_init () - intialise mylib template driver
 * s4743556_reg_rgb_brightness_write () - Contorols brightness
 * s4743556_reg_rgb_brightness_read () - Read brightness
 * s4743556_reg_rgb_colour_set () - Sets colour of LED
 * s4743556_reg_rgb_colour_get () - returns colour of LED
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_rgb.h"

int percent_brightness;

/*
 * Initialize RGB LED (GPIO, PWM, etc.)
 */
void s4743556_reg_rgb_init() {
	__GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

	// E11 Red 
    GPIOE->MODER &= ~(0x03 << (11 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (11 * 2));   //Set for push pull
    GPIOE->OSPEEDR &= ~(0x03 << (11 * 2));
	GPIOE->OSPEEDR |= 0x02 << (11 * 2);  // Set for Fast speed
    GPIOE->OTYPER &= ~(0x01 << 11);       //Clear Bit for Push/Pull Output
    GPIOE->PUPDR &= ~(0x03 << (11 * 2));   //Clear Bits
    GPIOE->PUPDR |= ((0x01) << (11 * 2));  //Set for Pull down output

	//F10 Green 
    GPIOD->MODER &= ~(0x03 << (10 * 2));  
    GPIOD->MODER |= (0x01 << (10 * 2));
    GPIOD->OSPEEDR &= ~(0x03 << (10 * 2));
	GPIOD->OSPEEDR |= 0x02 << (10 * 2);  
    GPIOD->OTYPER &= ~(0x01 << 10);       
    GPIOD->PUPDR &= ~(0x03 << (10 * 2));   
    GPIOD->PUPDR |= ((0x01) << (10 * 2)); 

	//F15 Blue 
    GPIOF->MODER &= ~(0x03 << (15 * 2));  
    GPIOF->MODER |= (0x01 << (15 * 2));
    GPIOF->OSPEEDR &= ~(0x03 << (15 * 2));
	GPIOF->OSPEEDR |= 0x02 << (15 * 2);  
    GPIOF->OTYPER &= ~(0x01 << 15);       
    GPIOF->PUPDR &= ~(0x03 << (15 * 2));   
    GPIOF->PUPDR |= ((0x01) << (15 * 2)); 

	// E9 Brightness
    GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << (9 * 2));		//Set fast speed.
	GPIOE->PUPDR &= ~(0x03 << (9 * 2));				//Clear bits for no push/pull
	GPIOE->MODER &= ~(0x03 << (9 * 2));				//Clear bits
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (9 * 2));  	//Set Alternate Function Push Pull Mode

    GPIOE->AFR[1] &= ~((0x0F) << ((9 - 8) * 4));			//Clear Alternate Function for pin (higher ARF register)
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << ((9 - 8) * 4));	//Set Alternate Function for pin (higher ARF register) 

	// Timer 1 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Set the clock prescaler to 50kHz
	   SystemCoreClock is the system clock frequency */
	TIM1->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM1->CR1 &= ~TIM_CR1_DIR; 

	TIM1->ARR = PWM_PULSE_WIDTH_TICKS;
	TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(50);	//Set pulse width to 50% duty cycle.

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 	// Clear OC2M (Channel 2) 
	TIM1->CCMR1 |= (0x6 << 4); 		// Enable PWM Mode 1, upcounting, on Channel 2 
	TIM1->CCMR1 |= (TIM_CCMR1_OC1CE); 	// Enable output preload bit for channel 2 
	
	TIM1->CR1  |= (TIM_CR1_ARPE); 	// Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC1E; 	// Set CC2E Bit 
	TIM1->CCER |= TIM_CCER_CC1NE; 	// Clear CC2NE Bit for active high output 
	
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
}

/*
 * Control the brightness level (Bright - 0% to 100% - OFF) using PWM duty cycle
 */
void s4743556_reg_rgb_brightness_write(int level) {
    percent_brightness = 2 * (100 - level); // times 2 cause increamented by 5%
    if (percent_brightness > 100){
        percent_brightness = 0;
        TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(100);

    } else if (percent_brightness < 0) {
        percent_brightness = 0;
        TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(100);
    }
    
    TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(level);
}


/*
 * Read the brightness level (0% to 100%)
 */
int s4743556_reg_rgb_brightness_read() {
    return percent_brightness;
}

/*
 * Set the primary colour of the RGB LED using an RGB bit mask
 */
void s4743556_reg_rgb_colour_set(unsigned char rgb_mask) {
    int B_led = (rgb_mask & (1 << 0)) >> 0;
    if (B_led == 1){
        GPIOF->ODR |= (0x01 << 15);
    }
    else {
        GPIOF->ODR &= ~(0x01 << 15);
    }

    int G_led = (rgb_mask & (1 << 1)) >> 1;
    if (G_led == 1){
        GPIOD->ODR |= (0x01 << 10);
    }
    else {
        GPIOD->ODR &= ~(0x01 << 10);
    }

    int R_led = (rgb_mask & (1 << 2)) >> 2;
    if (R_led == 1){
        GPIOE->ODR |= (0x01 << 11);
    }
    else {
        GPIOE->ODR &= ~(0x01 << 11);
    }
}

/*
 * Return the current primary colour of the RGB LED as an RGB bit mask
 */
unsigned char s4743556_reg_rgb_colour_get() {
	int R_state = (GPIOE->IDR & (0x01 << 11)) ? 1 : 0;  
    int G_state = (GPIOD->IDR & (0x01 << 10)) ? 1 : 0;  
    int B_state = (GPIOF->IDR & (0x01 << 15)) ? 1 : 0;  

    // Create an RGB bit mask
    unsigned char rgb_mask = (R_state << 2) | (G_state << 1) | B_state;

    return rgb_mask;
}