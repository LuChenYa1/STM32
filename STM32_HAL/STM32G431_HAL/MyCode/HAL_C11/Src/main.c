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
#include "lcd_hal.h"
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
typedef enum
{
	Auto,
	Manual
}mode;
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
Key_State KeyState = Key_Check;
uint8_t KeyNum;
uint16_t Key_Press_Time = 20;

uint8_t Str[20];

float Duty_6 = 0.1f, Duty_7 = 0.1f;//0~1
mode Mode = Auto;

uint8_t Flag_Show_Select;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LCD_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float getADC_R37(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	
	return (adc * 3.3f / 4096);
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
  MX_TIM3_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htim17, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim17, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 100);
  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 100);


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
			  Flag_Show_Select = !Flag_Show_Select;
		  }
		  else if(KeyNum == 2)
		  {
			  if(Flag_Show_Select == 1)
			  {
				  Duty_6 += 0.1f;
				  if(Duty_6 >= 1.0f)Duty_6 = 0.1f;
			  }
		  }
		  else if(KeyNum == 3)
		  {
			  if(Flag_Show_Select == 1)
			  {
				  Duty_7 += 0.1f;
			      if(Duty_7 >= 1.0f)Duty_7 = 0.1f;
			  }
		  }
		  else if(KeyNum == 4)
		  {
			  if(Mode == Auto)Mode = Manual;
			  else Mode = Auto;
		  }
		  Key_Press_Time = 0;
	  }
	  
	  
	  if(Mode == Auto)
	  {
		  LED_ON(1);
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (getADC_R37() / 3.3f) * 1000);
		  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (getADC_R37() / 3.3f) * 1000);
	  }
	  else 
	  {
		  LED_OFF(1);
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, Duty_6 * 1000);
		  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, Duty_7 * 1000);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void LCD_Show(void)
{
	if(Flag_Show_Select == 0)
	  {
		  LED_ON(2);
		  LCD_DisplayStringLine(Line0, (uint8_t *)"      Data");
		  LCD_DisplayStringLine(Line1, (uint8_t *)"                    ");
		  sprintf((char *)Str, "    V:%.2fV    ", getADC_R37());
		  LCD_DisplayStringLine(Line2, Str);
		  LCD_DisplayStringLine(Line3, (uint8_t *)"                    ");
		  if(Mode == Auto)LCD_DisplayStringLine(Line4, (uint8_t *)"    Mode:AUTO    ");
		  else LCD_DisplayStringLine(Line4, (uint8_t *)"    Mode:MANU    ");
		  LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
	  }
	  else 
	  {
		  LED_OFF(2);
		  LCD_DisplayStringLine(Line0, (uint8_t *)"      Para");
		  LCD_DisplayStringLine(Line1, (uint8_t *)"                    ");
		  sprintf((char *)Str, "    PA6:%.0f%%  ", Duty_6 * 100.0f);
		  LCD_DisplayStringLine(Line2, Str);
		  LCD_DisplayStringLine(Line3, (uint8_t *)"                    ");
		  sprintf((char *)Str, "    PA7:%.0f%%     ", Duty_7 * 100.0f);
		  LCD_DisplayStringLine(Line4, Str);
		  LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
		  LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
	  }
}
void Key_Scan(void)
{
	switch(KeyState)
	{
		case Key_Check:
		{
			if(K1 == Press || K2 == Press|| K3 == Press || K4 == Press)
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
			if(K1 == Press || K2 == Press|| K3 == Press || K4 == Press)
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
