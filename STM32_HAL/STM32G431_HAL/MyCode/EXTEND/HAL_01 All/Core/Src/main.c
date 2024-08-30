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
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include <stdio.h>
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

#define Press GPIO_PIN_RESET
#define Release GPIO_PIn_SET
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Str[20], Num, RX[10];
uint8_t ShowTime[9] = "HH:MM:SS";
uint8_t ShowDate[11] = "YYYY:MM:DD";
Key_State KeyState = Key_Check;
uint8_t KeyNum;
uint16_t Key_Press_Time = 20, Flag, Freq_Value_3, Freq_Value_2, Duty_Value_2, Duty_2;
float Freq_3, Freq_2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float getADC_R37(void);
void RTC_Show(void);
void RTC_Set(uint8_t YYYY, uint8_t OO, uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(++Flag == 1000)
	{
		Flag = 0;
		LED_TRG(8);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit_IT(&huart1, RX, 10);
	LCD_DisplayStringLine(Line0, RX);
	HAL_UART_Receive_IT(&huart1, RX, 10);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim -> Instance == TIM3)
	{
		Freq_Value_3 = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SetCounter(&htim3, 0);
		Freq_3 = 1000000.0f / Freq_Value_3;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	}
	else if(htim -> Instance == TIM2)
	{
		if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			Freq_Value_2 = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
			Freq_2 = 1000000.0f / Freq_Value_2;
		}
		else if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Duty_Value_2 = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
			Duty_2 = Duty_Value_2 * 100 / Freq_Value_2;
		}
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
  MX_ADC2_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_Base_Start_IT(&htim6);
  __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
  
  HAL_UART_Receive_IT(&huart1, RX, 10);
  
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
  
//  x24C02_Write(0x01, 0);
  
  Num = x24C02_Read(0x01);
  x24C02_Write(0x01, Num + 1);
  sprintf((char *)Str, "Num:%d", Num);
  LCD_DisplayStringLine(Line3, (uint8_t *)Str);
  
  RTC_Set(24, 5, 20, 20, 55, 59);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  sprintf((char *)Str, "ADC:%.2fV", getADC_R37());
	  LCD_DisplayStringLine(Line5, (uint8_t *)Str);
	  
	  sprintf((char *)Str, "Freq_3:%.2fHz", Freq_3);
	  LCD_DisplayStringLine(Line6, (uint8_t *)Str);
	  sprintf((char *)Str, "Duty_2:%d%%", Duty_2);
	  LCD_DisplayStringLine(Line7, (uint8_t *)Str);
	  
	  if(KeyState == Key_Over)
	  {
		  KeyState = Key_Check;
		  if(KeyNum == 1)
		  {
			  LED_ON(7);
		  }
		  else if(KeyNum == 2 && Key_Press_Time >= 1000)
		  {
			  LED_OFF(7);
		  }
		  else if(KeyNum == 3)
		  {
			  
		  }
		  else if(KeyNum == 4)
		  {
			  
		  }
		  Key_Press_Time = 0;
	  }
	  
	  RTC_Show();
	  
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
float getADC_R37(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return adc * 3.3f / 4096;
}

void RTC_Show(void)
{
	RTC_TimeTypeDef sTimeStructure;
	RTC_DateTypeDef sDateStructure;
	
	HAL_RTC_GetTime(&hrtc, &sTimeStructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDateStructure, RTC_FORMAT_BIN);
	
	sprintf((char *)ShowTime, "%2d:%2d:%2d", sTimeStructure.Hours, sTimeStructure.Minutes, sTimeStructure.Seconds);
	LCD_DisplayStringLine(Line8, ShowTime);
	sprintf((char *)ShowDate, "%2d:%2d:%2d", 2000 + sDateStructure.Year, sDateStructure.Month, sDateStructure.Date);
	LCD_DisplayStringLine(Line9, ShowDate);
}

void RTC_Set(uint8_t YYYY, uint8_t OO, uint8_t DD, uint8_t HH, uint8_t MM, uint8_t SS)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
		
	sTime.Hours = HH;
	sTime.Minutes = MM;
	sTime.Seconds = SS;
	
	sDate.Year = YYYY;
	sDate.Month = OO;
	sDate.Date = DD;
		
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);	
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
}

void Key_Scan(void)
{
	switch (KeyState)
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
