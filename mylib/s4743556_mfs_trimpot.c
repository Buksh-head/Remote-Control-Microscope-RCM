 /** 
 **************************************************************
 * @file mylib/s4743556_mfs_trimpot.c
 * @author Adnaan Buksh - 47435568
 * @date 01032024
 * @brief MFS Trimpot Mylib Register Driver
 ***************************************************************
   * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4743556_reg_mfs_trimpot_init () - intialise mylib template driver
 * s4743556_reg_mfs_trimpot_get () -  current Trimpot input value
 ***************************************************************
 */

#include <stdint.h>
#include "s4743556_mfs_trimpot.h"

ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef AdcChanConfig;

/*
 * Initialise GPIO pins and ADC
 */
void s4743556_reg_mfs_trimpot_init () {
    // Enable the GPIO Clock
  	__GPIOA_CLK_ENABLE();
  
	// Initalise PA3 as an Analog input.
  	GPIOA->MODER |= (0x03 << (3 * 2));			//Set bits for Analog input mode

  	GPIOA->OSPEEDR &= ~(0x03<<(3 * 2));
	GPIOA->OSPEEDR |= 0x02<<(3 * 2);  			// Fast speed

	GPIOA->PUPDR &= ~(0x03 << (3 * 2));			//Clear bits for no push/pull

	__ADC1_CLK_ENABLE();						//Enable ADC1 clock

	AdcHandle.Instance = (ADC_TypeDef *)(ADC1_BASE);						//Use ADC1 (For PA3)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandle.Init.ScanConvMode          = DISABLE;
	AdcHandle.Init.ContinuousConvMode    = DISABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion   = 0;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandle.Init.NbrOfConversion       = 1;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	AdcHandle.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandle);		//Initialise ADC

	// Configure ADC Channel
	AdcChanConfig.Channel = ADC_CHANNEL_3;					//PA3 has Analog Channel 3 connected
	AdcChanConfig.Rank         = 1;
	AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig.Offset       = S4743556_REG_MFS_TRIMPOT_ZERO_CAL_OFFSET;

	HAL_ADC_ConfigChannel(&AdcHandle, &AdcChanConfig);
    
}

/*
 * Read the current Trimpot input value (raw ADC value).
 */
int s4743556_reg_mfs_trimpot_get () {
	HAL_ADC_Start(&AdcHandle); // Start ADC conversion
	// Wait for ADC conversion to finish
	while (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK);

    return AdcHandle.Instance->DR;
}