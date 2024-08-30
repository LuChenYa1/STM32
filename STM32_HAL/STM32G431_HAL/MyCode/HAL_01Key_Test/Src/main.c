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
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_hal.h"
#include <stdio.h>
#include <string.h>
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

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define B1  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
#define B2  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define B3  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define B4  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

Key_State KeyState = Key_Check;
uint8_t KeyNum, Num;
uint32_t Key_Press_Time = 20;
uint8_t Str[20], RX[20];
//uint8_t Key3_Show[20] = "KEY3_SHOW";
uint16_t TIM_Time;
uint8_t cc2_Value;
uint8_t Freq_Value_R40, Duty_Value_R40;
uint8_t Freq_R40, Duty_R40;
uint8_t RecCount;

uint8_t ShowTime[10] = "HH:MM:SS";
uint8_t ShowDate[10] = "YYYY:MM:DD";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
void LED_Close(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器6回调函数
{
	if(++TIM_Time == 500)
	{
		TIM_Time = 0;
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	}
}

//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef * htim)//定时器2输入捕获回调函数
{
//	if(htim -> Instance  == TIM2)//先分后不分
//	{
//		cc2_Value = __HAL_TIM_GET_COUNTER(&htim2);
//		__HAL_TIM_SetCounter(&htim2,0);
//		f40 = 1000000 / cc2_Value;
//		HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);			
//	}
	
	if(htim -> Instance  == TIM2)
	{
		if(htim ->Channel == HAL_TIM_ACTIVE_CHANNEL_1)//频率
		{
			Freq_Value_R40 = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
			Freq_R40 = 1000000 / Freq_Value_R40;
		}
		else if(htim ->Channel == HAL_TIM_ACTIVE_CHANNEL_2)//占空比
		{
			Duty_Value_R40 = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
			Duty_R40 = Duty_Value_R40 * 100 / Freq_Value_R40;
		}
	}
}

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef * huart)
{
	RecCount = 10 - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);//重点记忆
	HAL_UART_Transmit_DMA(&huart1, RX, RecCount);
	RecCount = 0;
	LCD_DisplayStringLine(Line2, RX);
	HAL_UART_Receive_DMA(&huart1, RX, 10);
}

uint16_t getADC_R37(void)
{
	uint16_t adc;
	
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	
	return adc;
}

void RTC_Show(uint8_t * ShowTime, uint8_t * ShowDate)
{
	RTC_TimeTypeDef sTimeStructure;
	RTC_DateTypeDef sDateStructure;
	
	HAL_RTC_GetTime(&hrtc, &sTimeStructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDateStructure, RTC_FORMAT_BIN);
	
	sprintf((char *)ShowTime, "%2d:%2d:%2d", sTimeStructure.Hours, sTimeStructure.Minutes, sTimeStructure.Seconds);
    LCD_DisplayStringLine(Line4, ShowTime);
	
	sprintf((char *)ShowDate, "%2d-%2d-%2d", 2000 + sDateStructure.Year, sDateStructure.Month, sDateStructure.Date);
    LCD_DisplayStringLine(Line5, ShowDate);
}
void RTC_SetTime(uint8_t HH, uint8_t MM, uint8_t SS)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = HH;
	sTime.Minutes = MM;
	sTime.Seconds = SS;
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}
void RTC_SetDate(uint8_t YY, uint8_t MM, uint8_t DD)
{
	RTC_DateTypeDef sDate = {0};
	sDate.Year = YY;
	sDate.Month = MM;
	sDate.Date = DD;
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
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
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  LED_Close();
  
  //LCD初始化
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
//  for(uint8_t i = 0; i < 9; i++)
//  {
//	  LCD_DisplayChar(1*24, 320-(16*i)-160, Key3_Show[i]);
//  }
  LCD_DisplayStringLine(Line1, (uint8_t *)"           KEY3_SHOW");
  LCD_DisplayStringLine(Line2, (uint8_t *)"           UART_SHOW");
  
  //定时器6启动
  HAL_TIM_Base_Start_IT(&htim6);
  __HAL_TIM_CLEAR_IT(&htim6, TIM_FLAG_UPDATE);
  
  //PWM配置
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_2);
  
  __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 500);//设置占空比
  __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 700);
  
  //IC配置
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

  //串口
  HAL_UART_Receive_DMA(&huart1, RX, 10);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  
  
  //I2C_EEPROM
//  x24C02_Write(0x02, 0);
	Num = x24C02_Read(0x02);//每复位/上电一次，数字加一
	x24C02_Write(0x02, ++Num);
//  错误：1、配置PB6、PB7  2、对存储器写入数据应在while外面  3、按压时间清零，写在while里面，if键值判断最后 
	sprintf((char *)Str, "EEPROM:%d  ", Num);
	LCD_DisplayStringLine(Line0, (uint8_t *)Str);
	
  //RTC初始化
  RTC_SetDate(24, 3, 21);
  RTC_SetTime(18, 37, 50);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(KeyState == Key_Over)
	  {
		  KeyState = Key_Check;
		  if(KeyNum == 1 && Key_Press_Time <= 1000)
		  {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
     		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | 
			  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		  }
		  if(KeyNum == 2 && Key_Press_Time >= 1200)
		  {
			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
			  HAL_Delay(300);
			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		  }
		  if(KeyNum == 3)
		  {
			  LCD_DisplayStringLine(Line1, (uint8_t *)"1234567890");
		  }
		  Key_Press_Time = 0;//清零位置很重要，决定按压时间检测
	  }
	  
//	sprintf((char *)Str, "  Freq(R40):%dHz", Freq_R40);
//	LCD_DisplayStringLine(Line8, (uint8_t *)Str);  
//	sprintf((char *)Str, "  Duty(R40):%d%%", Duty_R40);
//	LCD_DisplayStringLine(Line9, (uint8_t *)Str);  
//	HAL_Delay(200);	//防止刷新过快

	  //错误：LCD和LED只能同时使用一个，该句使LCD一直使用，导致LED相关功能失效
      sprintf((char *)Str, "ADC(R37):%.2fV", getADC_R37() * 3.3f / 4096 );
	  LCD_DisplayStringLine(Line3, (uint8_t *)Str);
	  
	  RTC_Show(ShowTime, ShowDate);
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
void Key_Scan(void)
{
	switch(KeyState)
	{
		case Key_Check:
		{
			if(B1 == GPIO_PIN_RESET || B2 == GPIO_PIN_RESET || B3 == GPIO_PIN_RESET || B4 == GPIO_PIN_RESET)
			{
					KeyState = Key_Press;
			}
		}
		break;
		case Key_Press:
		{
			if(B1 == GPIO_PIN_RESET){KeyNum = 1;}
			if(B2 == GPIO_PIN_RESET){KeyNum = 2;}
			if(B3 == GPIO_PIN_RESET){KeyNum = 3;}
			if(B4 == GPIO_PIN_RESET){KeyNum = 4;}
			KeyState = Key_Release;
		}
		break;
		case Key_Release:
		{
			if(B1 == GPIO_PIN_RESET || B2 == GPIO_PIN_RESET || B3 == GPIO_PIN_RESET || B4 == GPIO_PIN_RESET)
			{
				Key_Press_Time += 10;
			}
			else
			{
				KeyState = Key_Over;
			}
		}
		break;
		default:{break;}
	}
}

void LED_Close(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | 
					  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
