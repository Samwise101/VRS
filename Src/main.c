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
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(const uint8_t* data, uint8_t lenght);


/* Space for your global variables. */

	uint16_t upperCaseLetterNum = 0;
	uint16_t lowerCaseLetterNum = 0;
	uint8_t correctString = 0;
	uint8_t startWaitForEndChar = 0;
	uint8_t count = 0;
	uint8_t parsingPos = 0;
	uint8_t dataToParse[35];

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  USART2_RegisterCallback(proccesDmaData);
  /* Space for your local variables, callback registration ...*/

  	  //type your code here:
  uint8_t transmitionBuffer[DMA_USART2_BUFFER_SIZE];

  while (1)
  {
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 0.5Hz.
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */
	  memset(transmitionBuffer, 0, DMA_USART2_BUFFER_SIZE);
	  snprintf(transmitionBuffer,
			  sizeof(transmitionBuffer),
			  "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %.1f%\n\r",
			  DMA_USART2_BUFFER_SIZE,DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6),
			  ((DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6))*100)/DMA_USART2_BUFFER_SIZE);

	  /* Valid text string information transmission.
	   * Transmission frequency - when new valid string is received.
	   * Message format - "Valid string: %s, lower-case: %d, upper-case: %d"
	   * Example message (what I wish to see in terminal) - Valid string: Platn15uborZnakov, lower-case: 13, upper-case: 2
	   */
	  if(correctString){
		  correctString = 0;
		  count = 0;
		  memset(transmitionBuffer, 0, DMA_USART2_BUFFER_SIZE);
		  snprintf(transmitionBuffer, DMA_USART2_BUFFER_SIZE, "Valid string: %s, lower-case: %d, upper-case: %d\n\r", dataToParse, lowerCaseLetterNum, upperCaseLetterNum);

		  upperCaseLetterNum = 0;
		  lowerCaseLetterNum = 0;
		  memset(dataToParse, 0, sizeof(dataToParse));
	  }


  	  	  	  //type your code here:
	  USART2_PutBuffer(transmitionBuffer, sizeof(transmitionBuffer));
	  LL_mDelay(2000);
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(const uint8_t* data, uint8_t lenght)
{
	if(data[0] == '#' && !startWaitForEndChar && !correctString){
		count = 0;
		startWaitForEndChar = 1;
	}
	if(startWaitForEndChar && !correctString){
		for(uint8_t i=0; i < lenght; i++){
			if(data[i] != '$' && count < sizeof(dataToParse)){
				dataToParse[count] = data[i];
				count++;
			}
			else if(data[i] == '$' && count < sizeof(dataToParse)){
				correctString = 1;
				startWaitForEndChar = 0;
				dataToParse[count] = data[i];
				break;
			}
			else if(data[i] != '$' && count >= sizeof(dataToParse)){
				correctString = 0;
				startWaitForEndChar = 0;
				memset(dataToParse, 0, sizeof(dataToParse));
				count = 0;
				break;
			}
		}
	}
	if(correctString){
		for(uint8_t i=0; i<strlen(dataToParse); i++){
			if(dataToParse[i] >= 65 && dataToParse[i] <= 90){
				upperCaseLetterNum++;
			}
			else if(dataToParse[i] >= 97 && dataToParse[i] <= 122){
				lowerCaseLetterNum++;
			}
		}
	}
}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
