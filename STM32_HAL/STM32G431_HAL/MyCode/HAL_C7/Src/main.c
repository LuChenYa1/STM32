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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_hal.h"
#include "lcd_hal.h"
#include "string.h"
#include <stdio.h>
#include "led.h"
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
uint8_t Str[20] = "              ";
uint8_t RX[1] = " ";
Key_State KeyState;
uint8_t KeyNum;
uint16_t Key_Press_Time = 20;
uint8_t RecCount;

uint8_t Flag_ADC;
uint8_t LCD_Select;
uint8_t Level, Level_Past, Limit_1 = 30, Limit_2 = 50, Limit_3 = 70;
float Volt, Height;
uint8_t Flag_Limit_Select;
uint8_t Flag_Level_LED2, Flag_LED3, Flag_Level_Past, Flag_State;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern  DMA_HandleTypeDef  hdma_usart1_rx;
extern  DMA_HandleTypeDef  hdma_usart1_tx;

void LCD_HighLight(uint8_t Line, uint8_t *LCD_Show, uint8_t pos, uint8_t add);
void LCD_Show(void);
float getADC_R37(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
{
	RecCount = 1 - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)RX, 1);
	RecCount = 0;
	LED_TRG(6);
	if(RX[0] == 'C')
	{
		sprintf((char *)Str, "C:H%.0f+L %d\r\n", Height, Level);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
		RX[0] = ' ';
		Flag_LED3 = 1;
	}
	else if(RX[0] == 'S')
	{
		sprintf((char *)Str, "S:TL%d+TM%d+TH%d\r\n", Limit_1, Limit_2, Limit_3);
		HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
		RX[0] = ' ';
		Flag_LED3 = 1;
	}
	
	if(Flag_Level_Past == 1)
	{
		if(Level > Level_Past)
		{
			sprintf((char *)Str, "A:H%.0f+L %d+D\r\n", Height, Level);
			HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
		}
		else if(Level < Level_Past)
		{
			sprintf((char *)Str, "A:H%.0f+L %d+U\r\n", Height, Level);
			HAL_UART_Transmit_DMA(&huart1, Str, strlen((char *)Str));
		}
	}
	HAL_UART_Receive_DMA(&huart1, RX, 1);
}
/* US
ER CODE END 0 */

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
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  HAL_UART_Receive_DMA(&huart1, RX, 1);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);


//	x24C02_Write(0x01, 30);
//	x24C02_Write(0x02, 50);
//	x24C02_Write(0x03, 70);
  Limit_1 = x24C02_Read(0x01);
  Limit_2 = x24C02_Read(0x02);
  Limit_3 = x24C02_Read(0x03);
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
			  LCD_Select = !LCD_Select;
			  Flag_State = !Flag_State;
		  }
		  else if(KeyNum == 2)
		  {
			  Flag_Limit_Select ++;
			  Flag_Limit_Select %= 3;
		  }
		  else if(KeyNum == 3)
		  {
			  if(Flag_Limit_Select == 0)
			  {
				  Limit_1 += 5;
				  if(Limit_1 == 100)Limit_1 = 95;
			  }
			  else if(Flag_Limit_Select == 1)
			  {
				  Limit_2 += 5;
				  if(Limit_2 == 100)Limit_2 = 95;
			  }
			  else if(Flag_Limit_Select == 2)
			  {
				  Limit_3 += 5;
				  if(Limit_3 == 100)Limit_3 = 95;
			  }
			  x24C02_Write(0x01, Limit_1);
			  x24C02_Write(0x02, Limit_2);
			  x24C02_Write(0x03, Limit_3);
		  }
		  else if(KeyNum == 4)
		  {
			  if(Flag_Limit_Select == 0)
			  {
				  Limit_1 -= 5;
				  if(Limit_1 == 0)Limit_1 = 5;
			  }
			  else if(Flag_Limit_Select == 1)
			  {
				  Limit_2 -= 5;
				  if(Limit_2 == 0)Limit_2 = 5;
			  }
			  else if(Flag_Limit_Select == 2)
			  {
				  Limit_3 -= 5;
				  if(Limit_3 == 0)Limit_3 = 5;
			  }
			  x24C02_Write(0x01, Limit_1);
			  x24C02_Write(0x02, Limit_2);
			  x24C02_Write(0x03, Limit_3);
		  }
		  Key_Press_Time = 0;
	  }
	  
	  
	  
	  Level_Past = Level;
	  if(Height <= Limit_1)Level = 0;
	  else if(Height > Limit_1 && Height <= Limit_2)Level = 1;
	  else if(Height > Limit_2 && Height <= Limit_3)Level = 2;
	  else if(Height > Limit_3)Level = 3;
	  if(Level_Past != Level)
	  {
		  Flag_Level_Past = 1;
	  }
	  
	  if(Flag_ADC == 1)
	  {
		  Volt = getADC_R37();
		  Height = (Volt * 100.0f / 3.3f);
	  }
	  
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
	if(LCD_Select == 0)
	  {
		  LCD_DisplayStringLine(Line0, (uint8_t *)"                   ");
		  LCD_DisplayStringLine(Line1, (uint8_t *)"    Liquid Level   ");
		  LCD_DisplayStringLine(Line2, (uint8_t *)"                   ");
		  sprintf((char *)Str, "     Height:%.0fcm    ", Height);
		  LCD_DisplayStringLine(Line3, Str);
		  LCD_DisplayStringLine(Line4, (uint8_t *)"                   ");
		  sprintf((char *)Str, "     ADC:%.2f     ", Volt);
		  LCD_DisplayStringLine(Line5, Str);
		  LCD_DisplayStringLine(Line6, (uint8_t *)"                   ");
		  sprintf((char *)Str, "     Level:%d        ", Level);
		  LCD_DisplayStringLine(Line7, Str);
		  LCD_DisplayStringLine(Line8, (uint8_t *)"                   ");
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                   ");
	  }
	  else if(LCD_Select == 1)
	  {
		  LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line1, (uint8_t *)"    Parameter Setup ");
		  LCD_DisplayStringLine(Line2, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
		  
		  
		  if(Flag_Limit_Select == 0)
		  {
			  sprintf((char *)Str, "  Threshold 1:%dcm  ", Limit_1);
			  LCD_HighLight(3, Str, 0, 20); 
			  sprintf((char *)Str, "  Threshold 2:%dcm  ", Limit_2);
			  LCD_DisplayStringLine(Line5, Str); 
			  sprintf((char *)Str, "  Threshold 3:%dcm  ", Limit_3);
			  LCD_DisplayStringLine(Line7, Str);  
		  }
		  else if(Flag_Limit_Select == 1)
		  {
			  sprintf((char *)Str, "  Threshold 1:%dcm  ", Limit_1);
			  LCD_DisplayStringLine(Line3, Str);
			  sprintf((char *)Str, "  Threshold 2:%dcm  ", Limit_2);
			  LCD_HighLight(5, Str, 0, 20); 
			  sprintf((char *)Str, "  Threshold 3:%dcm  ", Limit_3);
			  LCD_DisplayStringLine(Line7, Str);  
		  }
		  else if(Flag_Limit_Select == 2)
		  {
			  sprintf((char *)Str, "  Threshold 1:%dcm  ", Limit_1);
			  LCD_DisplayStringLine(Line3, Str);
			  sprintf((char *)Str, "  Threshold 2:%dcm  ", Limit_2);
			  LCD_DisplayStringLine(Line5, Str); 
			  sprintf((char *)Str, "  Threshold 3:%dcm  ", Limit_3);
			  LCD_HighLight(7, Str, 0, 20);  
		  }
		  
	  }
}
void LCD_HighLight(uint8_t Line, uint8_t *LCD_Show, uint8_t pos, uint8_t add)
{
	uint8_t i;
	for(i = 0; i < 20; i++)
	{
		if(i != pos)LCD_DisplayChar(24 * Line, 320 - (16 * i), LCD_Show[i]);
		else i += add;
	}
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	for(i = 0; i < add; i++)
	{
		LCD_DisplayChar(24 * Line, 320 - (16 * (i + pos)), LCD_Show[i + pos]);
	}
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
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

float getADC_R37(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return (adc * 3.3f / 4096.0f);
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
