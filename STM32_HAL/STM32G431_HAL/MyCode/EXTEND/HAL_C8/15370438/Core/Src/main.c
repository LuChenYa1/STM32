/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include <stdio.h>
#include "lcd.h"
#include "i2c_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	Key_Check,
	Key_Press,
	Key_Release,
	Key_Over
}Key_State;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define K1 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
#define K2 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define K3 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define K4 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define Press   GPIO_PIN_RESET
#define Release GPIO_PIN_SET
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Key_State KeyState = Key_Check;
uint8_t KeyNum, Num;
uint16_t Key_Press_Time = 20, Preq_Value_3, Preq_3, Preq_Value_2, Preq_2, Period;
uint8_t Str[20], ShowSelect, MutiplyPreq, SplitPreq, PreqSelect;
float Vol_37, Vol_38;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
float getADC_R37(void);
float getADC_R38(void);
void HighLight(uint8_t Linex, uint8_t * LCDShow, uint8_t pos, uint8_t add);
void LCD_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef * htim)
{
	if(htim->Instance == TIM3)
	{
		Preq_Value_3 = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SetCounter(&htim3, 0);
		Preq_3 = 1000000 / Preq_Value_3;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	}
	else if(htim->Instance == TIM2)
	{
		Preq_Value_2 = __HAL_TIM_GET_COUNTER(&htim2);
		__HAL_TIM_SetCounter(&htim2, 0);
		Preq_2 = 1000000 / Preq_Value_2;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
 
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  
  HAL_TIM_PWM_Start_IT(&htim16, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim16, TIM_CHANNEL_1);
  __HAL_TIM_SetAutoreload(&htim16, 999);
  
//  x24C02_Write(0x01, 0);
//  x24C02_Write(0x02, 0);
  SplitPreq = x24C02_Read(0x01);
  MutiplyPreq = x24C02_Read(0x02); 
  SplitPreq = x24C02_Read(0x01);
  MutiplyPreq = x24C02_Read(0x02); 
  MutiplyPreq = x24C02_Read(0x02); 
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {  
	  if(KeyState == Key_Over)
	  {
		  KeyState = Key_Check;
		  if(KeyNum == 1)
		  {
			 ShowSelect = !ShowSelect;
			 if(ShowSelect == 0)
			 {
				 x24C02_Write(0x01, SplitPreq);
				 x24C02_Write(0x02, MutiplyPreq);
				 x24C02_Write(0x01, SplitPreq);
				 x24C02_Write(0x02, MutiplyPreq);
			 }
		  }
		  else if(KeyNum == 2)
		  {
			 PreqSelect = !PreqSelect;
		  }
		  else if(KeyNum == 3)
		  {
			  if(ShowSelect == 1)
			  {
				  if(PreqSelect == 0)
				  {
					  SplitPreq ++;
					  if(SplitPreq >= 5)SplitPreq = 1;
				  }
				  else 
				  {
					  MutiplyPreq ++;
					  if(MutiplyPreq >= 5)MutiplyPreq = 1;
				  }
			  }
			  
		  }
		  else if(KeyNum == 4)
		  {
			  if(ShowSelect == 1)
			  {
				  if(PreqSelect == 0)
				  {
					  SplitPreq --;
					  if(SplitPreq == 0)SplitPreq = 4;
				  }
				  else 
				  {
					  MutiplyPreq --;
					  if(MutiplyPreq == 0)MutiplyPreq = 4;
				  }
			  }
		  }
	  }
	  
	  if(ShowSelect == 1)LED_ON(1);
	  else LED_OFF(1);
	  if(Vol_37 > Vol_38)LED_ON(8);
	  else LED_OFF(8);
	  
	  Period = (uint16_t)((SplitPreq * 1.0f / Preq_2) * 1000000 - 1);
	  __HAL_TIM_SetAutoreload(&htim16, Period);
	  if(ShowSelect == 1)__HAL_TIM_SetCompare(&htim16, TIM_CHANNEL_1, 0);
	  else __HAL_TIM_SetCompare(&htim16, TIM_CHANNEL_1, (Period + 1) / 2);
	  
	  sprintf((char *)Str, "%d", Period);
	  LCD_DisplayStringLine(Line0, Str);
	  
	  Vol_37 = getADC_R37();
	  Vol_38 = getADC_R38();
	  
	  if(MutiplyPreq == 0 || MutiplyPreq > 4)MutiplyPreq = 1;
	  if(SplitPreq == 0 || SplitPreq > 4)SplitPreq = 1;
	  
	  LCD_Show();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HighLight(uint8_t Linex, uint8_t * LCDShow, uint8_t pos, uint8_t add)
{
	uint8_t i;
	for(uint8_t i = 0; i < 20; i++)
	{
		if(i != pos)
		{
			LCD_DisplayChar(Linex * 24, 320 - (16 * i), LCDShow[i]);
		}
		else if(i == pos)i += add;
	}
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	for(i = 0; i < add; i ++)
	{
		LCD_DisplayChar(24 *Linex, 320 - (16 * (i + pos)), LCDShow[i + pos]);
	}
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
}

void LCD_Show(void)
{
	if(ShowSelect == 0)
	  {
		  LCD_DisplayStringLine(Line2, (uint8_t *)"    Meature    ");
		  
		  sprintf((char *)Str, "PULS1:%dHz       ", Preq_2);
		  LCD_DisplayStringLine(Line4, (uint8_t *)Str);
		  sprintf((char *)Str, "PULS2:%dHz       ", Preq_3);
		  LCD_DisplayStringLine(Line5, (uint8_t *)Str);
		  
		  sprintf((char *)Str, "AO1:%.2fV           ", Vol_37);
		  LCD_DisplayStringLine(Line6, (uint8_t *)Str);
		  sprintf((char *)Str, "AO2:%.2fV           ", Vol_38);
		  LCD_DisplayStringLine(Line7, (uint8_t *)Str);
		  
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                   1");
	  }
	  else 
	  {
		  LCD_DisplayStringLine(Line2, (uint8_t *)"    Set    ");
		  
		  if(PreqSelect == 0)
		  {
			  sprintf((char *)Str, "SplitPreq:%d         ", SplitPreq);
			  HighLight(4, (uint8_t *)Str, 0, 20);
			  sprintf((char *)Str, "MutiplyPreq:%d       ", MutiplyPreq);
			  LCD_DisplayStringLine(Line6, (uint8_t *)Str);
		  }
		  else
		  {
			  sprintf((char *)Str, "SplitPreq:%d         ", SplitPreq);
			  LCD_DisplayStringLine(Line4, (uint8_t *)Str);
			  
			  sprintf((char *)Str, "MutiplyPreq:%d       ", MutiplyPreq);
			  HighLight(6, (uint8_t *)Str, 0, 20);
			  
		  }
		  
		  LCD_DisplayStringLine(Line5, (uint8_t *)"                   ");
		  LCD_DisplayStringLine(Line7, (uint8_t *)"                   ");
		  
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                   2");
	  }
}

float getADC_R37(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return adc * 3.3f / 4096; 
}

float getADC_R38(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	adc = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return adc * 3.3f / 4096; 
}

void Key_Scan(void)
{
	switch(KeyState)
	{
		case Key_Check:
		{
			if(K1 == Press || K2 == Press || K3 == Press || K4 == Press)
			{
				KeyState = Key_Press;
			}
		}
		break;
		case Key_Press:
		{
			if(K1 == Press)KeyNum = 1;
			else if(K2 == Press)KeyNum = 2;
			else if(K3 == Press)KeyNum = 3;
			else if(K4 == Press)KeyNum = 4;
			KeyState = Key_Release;
		}
		break;
		case Key_Release:
		{
			if(K1 == Press || K2 == Press || K3 == Press || K4 == Press)
			{
				Key_Press_Time += 10;
			}
			else
			{
				KeyState = Key_Over;
			}
		}
		break;
		default:
		break;
	}
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
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
