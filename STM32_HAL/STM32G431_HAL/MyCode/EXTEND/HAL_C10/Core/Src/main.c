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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "lcd.h"
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
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define K1 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
#define K2 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define K3 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define K4 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define Press GPIO_PIN_RESET
#define Release GPIO_PIN_SET

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t KeyNum, Str[20], RX[4];
Key_State KeyState = Key_Check;
uint16_t Key_Press_Time = 20, Freq_Value_3, Duty_Value_3, Freq_3, Duty_3, N;
float Vol_1, Vol_2, VIN, K;
uint8_t ShowSelect, T = 30, X, PramSelect, Flag_N, Flag_UART_Transmit, RecCount, Flag_LED;
extern DMA_HandleTypeDef hdma_usart1_rx;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
float getADC_R37(void);
float getADC_R38(void);
void HighLight(uint8_t Linex, uint8_t *LCD_Show, uint8_t pos, uint8_t add);
void LCD_Show(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	LED_TRG(4);
//	if(RX[0] == 'S' && RX[1] == 'T')
//	{
////		LED_TRG(4);
//		sprintf((char *)Str, "$%d\r\n", Freq_3);
//	    HAL_UART_Transmit_IT(&huart1, (uint8_t *)Str, 8);
//	}
//	HAL_UART_Receive_IT(&huart1, RX, 6);
//}

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
{
	RecCount = 10 - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if(RX[0] == 'S' && RX[1] == 'T')
	{
		sprintf((char *)Str, "$%d\r\n", Freq_3);
	    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)Str, strlen((char *)Str));
	}
	if(RX[0] == 'P' && RX[1] == 'A' && RX[2] == 'R' && RX[3] == 'A')
	{
		sprintf((char *)Str, "#%d,AO1\r\n", T);
	    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)Str, strlen((char *)Str));
	}
	RecCount = 0;
	HAL_UART_Receive_DMA(&huart1, RX, 10);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			Freq_Value_3 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
			Freq_3 = 1000000 / Freq_Value_3;
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Duty_Value_3 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2);
			Duty_3 = Duty_Value_3 * 100.0f / Freq_Value_3;
			K = Duty_3 * 1.0f / 100.0f;//K有问�?
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);

//  x24C02_Write(0x01, 0);

  HAL_UART_Receive_DMA(&huart1, RX, 10);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
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
				 if(Flag_N == 1)
				 {
					 N ++;
					 Flag_N = 0;
					 x24C02_Write(0x01, N);
				 }
			 }
		  }
		  else if(KeyNum == 2)
		  {
			  PramSelect = !PramSelect;
		  }
		  else if(KeyNum == 3)
		  {
			  if(PramSelect == 0)
			  {
				  T ++;
				  if(T == 41)T = 20;
			  }
			  else X = !X;
			  Flag_N = 1;
		  }
		  else if(KeyNum == 4)
		  {
			  if(PramSelect == 0)
			  {
				  T --;
				  if(T <= 19)T = 40;
			  }
			  else X = !X;
			  Flag_N = 1;
		  }
		  Key_Press_Time = 0;
	  }
	 
	  Vol_2 = getADC_R38();
	  Vol_1 = getADC_R37();
	  if(X == 0)VIN = Vol_1;
	  else VIN = Vol_2;
	  
	  if(VIN > K * 3.3f)
	  {
		  Flag_UART_Transmit = 1;
		  LED_ON(1);
	  }
	  else
	  {
		  Flag_UART_Transmit = 0;
		  LED_OFF(1);
	  }
	  
	  if(T < Freq_3)Flag_LED = 1;
	  else Flag_LED = 0;
	  
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void LCD_Show(void)
{
	if(ShowSelect == 0)//数据显示
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"       Main  ");
		  sprintf((char *)Str, "AO1: %.2fV    ", Vol_1);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "AO2: %.2fV    ", Vol_2);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "PWM2: %d%%    ", Duty_3);
		  LCD_DisplayStringLine(Line5, Str);
		  sprintf((char *)Str, "Temp: %d.C  ", Freq_3);
		  LCD_DisplayStringLine(Line6, Str);
		  sprintf((char *)Str, "N: %d         ", N);
		  LCD_DisplayStringLine(Line7, Str);
	  }
	  else //参数设置
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"       Para");
		  
		  if(PramSelect == 0)
		  {
			  sprintf((char *)Str, "T:%d                ", T);
			  HighLight(3, Str, 0, 20);
			  if(X == 0)LCD_DisplayStringLine(Line4, (uint8_t *)"X:AO1               ");
			  else LCD_DisplayStringLine(Line4, (uint8_t *)"X:AO2              ");
			  LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
			  LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
			  LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
		  }
		  else
		  {
			  sprintf((char *)Str, "T:%d                 ", T);
			  LCD_DisplayStringLine(Line3, Str);
			  if(X == 0)HighLight(4, (uint8_t *)"X:AO1               ", 0, 20);
			  else HighLight(4, (uint8_t *)"X:AO2               ", 0, 20);
			  LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
			  LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
			  LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
		  }
	  }
}

void HighLight(uint8_t Linex, uint8_t *LCD_Show, uint8_t pos, uint8_t add)
{
	uint8_t i;
	for(i = 0; i < 20; i++)
	{
		if(i != pos)LCD_DisplayChar(Linex * 24, 320 - (16 * i),LCD_Show[i]);
		else i += add;
	}
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	for(i = 0; i < add; i++)
	{
		LCD_DisplayChar(Linex * 24, 320 - (16 * (pos + i)),LCD_Show[i + pos]);
	}
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
}

float getADC_R37(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
    adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return (adc * 3.3f / 4096.0f);
}

float getADC_R38(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
    adc = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return (adc * 3.3f / 4096.0f);
}

void Key_Scan(void)
{
	switch (KeyState)
	{
		case Key_Check:
		{
			if(K1 == Press || K2 == Press ||K3 == Press ||K4 == Press)
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
			if(K1 == Press || K2 == Press ||K3 == Press ||K4 == Press)
			{
				Key_Press_Time += 10;
				if(Key_Press_Time >= 800 && K3 == Press)
				{
					if(PramSelect == 0)
					{
						T ++;
						if(T == 41)T = 20;
					}
				}
				if(Key_Press_Time >= 800 && K4 == Press)
				{
					if(PramSelect == 0)
					{
						T --;
						if(T == 19)T = 40;
					}
				}
			}
			else KeyState = Key_Over;
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
