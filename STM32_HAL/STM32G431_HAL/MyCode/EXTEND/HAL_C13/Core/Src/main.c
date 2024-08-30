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
#include "i2c_hal.h"
#include "led.h"
#include "lcd.h"
#include "stdio.h"
#include <string.h>
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
uint8_t KeyNum, RX[20], Str[20], RecCount, ShowSelect, X, Y, Flag_ADC_UPDATE, Mode_Multiply_Split, Show_PA4_PA5;
uint32_t Key_Press_Time = 20, Freq_Value, Freq, Period;
Key_State KeyState = Key_Check;
float Vol_PA5, Vol_PA4, A_4, A_5, T_4, T_5, H_4, H_5, N_VOL_4[100], sum_4, sum_5;
float N_VOL_5[100];
extern DMA_HandleTypeDef hdma_usart1_rx;
uint8_t N_4, N_5, Flag_LED_3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
void getADC_PA45(void);
void LCD_Show(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
{
	RecCount = 10 - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if(RX[0] == 'X')
	{
		sprintf((char *)Str, "X:%d\r\n", X);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
	}
	else if(RX[0] == 'Y')
	{
		sprintf((char *)Str, "Y:%d\r\n", Y);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
	}
	if(RX[0] == 'P' && RX[1] == 'A' && RX[2] == '1')
	{
		sprintf((char *)Str, "PA1:%d\r\n", Freq);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
	} 
	else if(RX[0] == 'P' && RX[1] == 'A' && RX[2] == '4')
	{
		sprintf((char *)Str, "PA4:%.2f\r\n", Vol_PA4);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
	} 
	else if(RX[0] == 'P' && RX[1] == 'A' && RX[2] == '5')
	{
		sprintf((char *)Str, "PA5:%.2f\r\n", Vol_PA5);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
	} 
	RecCount = 0;
	HAL_UART_Receive_DMA(&huart1, RX, 10);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Freq_Value = __HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SetCounter(&htim2, 0);
	Freq = 1000000 / Freq_Value;
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_UART_Receive_DMA(&huart1, RX, 10);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
  
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
  
//  x24C02_Write(0x00, 0);
//  x24C02_Write(0x01, 0);
  Y = x24C02_Read(0x00);
  X = x24C02_Read(0x01);
  X = x24C02_Read(0x01);
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
			  ShowSelect ++;
			  ShowSelect %= 3;
			  if(ShowSelect == 2)
			  {
				  x24C02_Write(0x00, Y);
				  x24C02_Write(0x01, X);
				  x24C02_Write(0x01, X);
				  
				  Show_PA4_PA5 = 0;
			  }
		  }
		  else if(KeyNum == 2)
		  {
			  if(ShowSelect == 1)
			  {
				  X++;
				  if(X == 5)X = 1;
			  }
		  }
		  else if(KeyNum == 3)
		  {
			  if(ShowSelect == 1)
			  {
				  Y++;
				  if(Y == 5)Y = 1;
			  }
		  }
		  else if(KeyNum == 4)
		  {
			  if(ShowSelect == 0)
			  {
				  Flag_ADC_UPDATE = 1;
				  N_4++;
				  N_5++;
			  }
			  else if(ShowSelect == 1)
			  {
				  Mode_Multiply_Split = !Mode_Multiply_Split;
			  }
			  else
			  {
				  if(Key_Press_Time > 1000)
				  {
					  if(Show_PA4_PA5 == 0)
					  {
						  N_4 = 0;A_4 = 0;T_4 = 0;H_4 = 0;
					  }
					  else 
					  {
						  N_5 = 0;A_5 = 0;T_5 = 0;H_5 = 0;
					  }
				  }
				  else
				  {
					  Show_PA4_PA5 = !Show_PA4_PA5;
				  }
			  }
		  }
		  Key_Press_Time = 0;
	  }
	
	  if(Mode_Multiply_Split == 0)Period = Freq_Value / X;
	  else Period = Freq_Value * X;
	  
	  __HAL_TIM_SetAutoreload(&htim3, Period);
	  __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, Period / 2);
//	  __HAL_TIM_SetAutoreload(&htim3, 999);
//	  __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 999 / 2);
	  
	  if(Flag_ADC_UPDATE == 1)
	  {
		  getADC_PA45();
		  N_VOL_4[N_4 - 1] = Vol_PA4;
		  N_VOL_5[N_5 - 1] = Vol_PA5;
		  
		  if(Vol_PA4 > A_4)A_4 = Vol_PA4;
		  if(Vol_PA5 > A_5)A_5 = Vol_PA5;
		  
		  if(N_4 == 1)T_4 = Vol_PA4;
		  else 
		  {
			  if(Vol_PA4 < T_4)T_4 = Vol_PA4;
		  }
		  if(N_5 == 1)T_5 = Vol_PA5;
		  else 
		  {
			  if(Vol_PA5 < T_5)T_5 = Vol_PA5;
		  }
		  
		  sum_4 += Vol_PA4;
          sum_5 += Vol_PA5;
          H_4 = sum_4 / N_4;
		  H_5 = sum_5 / N_5;
		  
		  Flag_ADC_UPDATE = 0;
	  }
	  
	  if(Mode_Multiply_Split == 0)LED_ON(1);
	  else LED_OFF(1);
	  if(Mode_Multiply_Split == 1)LED_ON(2);
	  else LED_OFF(2);
	  if(Vol_PA4 > Y * Vol_PA5)Flag_LED_3 = 1;
	  else Flag_LED_3 = 0;
	  
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
	if(ShowSelect == 0)//数据
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"         DATA   ");
		  sprintf((char *)Str, "     PA4=%.2f", Vol_PA4);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "     PA5=%.2f", Vol_PA5);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "     PA1=%d", Freq);
		  LCD_DisplayStringLine(Line5, Str);
	      LCD_DisplayStringLine(Line6, (uint8_t *)"              ");
	  }
	  else if(ShowSelect == 1)//参数
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"         PARA   ");
		  sprintf((char *)Str, "     X=%d            ", X);
		  LCD_DisplayStringLine(Line3, Str); 
		  sprintf((char *)Str, "     Y=%d            ", Y);
		  LCD_DisplayStringLine(Line4, Str);
		  LCD_DisplayStringLine(Line5, (uint8_t *)"              ");
		  LCD_DisplayStringLine(Line6, (uint8_t *)"              ");
	  }
	  else if(ShowSelect == 2)//记录
	  {
		  if(Show_PA4_PA5 == 0)
		  {
			  LCD_DisplayStringLine(Line1, (uint8_t *)"         REC-PA4");
			  sprintf((char *)Str, "     N=%d            ", N_4);
			  LCD_DisplayStringLine(Line3, Str); 
			  sprintf((char *)Str, "     A=%.2f            ", A_4);
			  LCD_DisplayStringLine(Line4, Str);
			  sprintf((char *)Str, "     T=%.2f            ", T_4);
			  LCD_DisplayStringLine(Line5, Str);
			  sprintf((char *)Str, "     H=%.2f            ", H_4);
			  LCD_DisplayStringLine(Line6, Str);
		  }
		  else
		  {
			  LCD_DisplayStringLine(Line1, (uint8_t *)"         REC-PA5");
			  sprintf((char *)Str, "     N=%d            ", N_5);
			  LCD_DisplayStringLine(Line3, Str); 
			  sprintf((char *)Str, "     A=%.2f            ", A_5);
			  LCD_DisplayStringLine(Line4, Str);
			  sprintf((char *)Str, "     T=%.2f            ", T_5);
			  LCD_DisplayStringLine(Line5, Str);
			  sprintf((char *)Str, "     H=%.2f            ", H_5);
			  LCD_DisplayStringLine(Line6, Str);			  
		  }
	  }
}
void getADC_PA45(void)
{
	uint16_t adc5, adc4;
	
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc5 = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc4 = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	
	Vol_PA5 = adc5 * 3.3f / 4096;
	Vol_PA4 = adc4 * 3.3f / 4096;
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
