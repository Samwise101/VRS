/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "assignment.h"

int main(void)
{
  /*
   *  DO NOT WRITE TO THE WHOLE REGISTER!!!
   *  Write to the bits, that are meant for change.
   */
   
  //Systick init
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);	

  /*
   * TASK - configure MCU peripherals so that button state can be read and LED will blink.
   * Button must be connected to the GPIO port A and its pin 3.
   * LED must be connected to the GPIO port A and its pin 4.
   *
   * In header file "assignment.h" define macros for MCU registers access and LED blink application.
   * Code in this file must use these macros for the peripherals setup.
   * Code of the LED blink application is already given so just the macros used in the application must be defined.
   */


  /* Enable clock for GPIO port A*/

	//type your code for GPIOA clock enable here:
    // str.148
  (RCC_AHBENR_REG) |= (uint32_t)(1 << RCC_REG_GPIOA_POS);

  /* GPIOA pin 3 and 4 setup */

  //type your code for GPIOA pins setup here:
  /*GPIO MODER register*/
  //Set mode for pin 4 - LED str.237 - 0x3 = 11 v bin
  (GPIOA_MODER_REG) &= ~(uint32_t)(0x3 << (LED_PIN_POS *2));
  (GPIOA_MODER_REG) |= (uint32_t)(1 << (LED_PIN_POS *2));
  //Set mode for pin 3
  (GPIOA_MODER_REG) &= ~(uint32_t)(0x3 << (BUTTON_PIN_POS *2));

  /*GPIO OTYPER register*/
  (GPIOA_OTYPER_REG) &= ~(1 << LED_PIN_POS);

  /*GPIO OSPEEDR register*/
  //Set Low speed for GPIOA pin 4
  (GPIOA_OSPEEDER_REG) &= ~(0x3 << (LED_PIN_POS *2));

  /*GPIO PUPDR register, reset*/
  //Set pull up for GPIOA pin 3 (input) (button)
  (GPIOA_PUPDR_REG) |= (1 << (BUTTON_PIN_POS *2));
  //Set no pull for GPIOA pin 4
  (GPIOA_PUPDR_REG) &= ~(0x3 << (LED_PIN_POS *2));

  previous_state = BUTTON_GET_STATE;

  while (1)
  {
	  edge = edgeDetect(BUTTON_GET_STATE, SAMPLE_COUNT);


	  if(edge == FALL){
		  if(~LED_GET_STATE)
			  LED_ON;
		  else
			  LED_OFF;
	  }
/*
	  if(!(BUTTON_GET_STATE))
	  {
		  // 0.25s delay
		  LL_mDelay(250);
		  LED_ON;
		  // 0.25s delay
		  LL_mDelay(250);
		  LED_OFF;
	  }
	  else
	  {
		  // 1s delay
		  LL_mDelay(1000);
		  LED_ON;
		  // 1s delay
		  LL_mDelay(1000);
		  LED_OFF;
	  }
*/
  }
}

/* USER CODE BEGIN 4 */

EDGE_TYPE edgeDetect(uint8_t pin_state, uint8_t samples)
{
	if(pin_state && !previous_state){
		previous_state = 1;
		count = 0;
	}
	else if(!pin_state && count < samples){
		previous_state = 0;
		count++;
		if(count == samples)
			return FALL;
	}
	return NONE;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
