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
	Upper,
	Lower,
	Normal
}_Status;
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
uint8_t Str[20];
Key_State KeyState = Key_Check;
uint16_t Key_Press_Time = 20;
uint8_t KeyNum;

uint8_t Flag_K1;
_Status Status = Normal;

float Vol_M = 2.4, Vol_m = 1.2;
uint8_t LED_U = 1, LED_L = 2; 
uint8_t Flag_Set;

uint8_t Flag_LEDU, Flag_LEDL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LCD_HighLight(uint8_t Line, uint8_t * LCD_Show, uint8_t pos, uint8_t add);
void LCD_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float getADC_R37(void)
{
	uint16_t adc;//0~4096
	
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
  /* USER CODE BEGIN 2 */
  LED_Init();
 
  LCD_Init();
  
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if(KeyState == Key_Over)
	{
		KeyState = Key_Check;
		if(KeyNum == 1)//设置界面
		{	
			Flag_K1 = !Flag_K1;
			
		}
		else if(KeyNum == 2)//选择
		{
			if(Flag_K1 == 1)
			{
				Flag_Set ++;
				Flag_Set %= 4;
			}
			
		}
		else if(KeyNum == 3)
		{
			if(Flag_K1 == 1)
			{
				if(Flag_Set == 0)
				{
					Vol_M += 0.3f;
					if(Vol_M >= 3.3f)Vol_M = 0;
				}
				else if(Flag_Set == 1)
				{
					Vol_m += 0.3f;
					if(Vol_m >= 3.3f)Vol_m = 0;
				}
				else if(Flag_Set == 2)
				{
					LED_U ++;
					if(LED_U == 9)LED_U = 1;
					if(LED_L == LED_U)
					{
						LED_U ++;
						if(LED_U == 9)LED_U = 1;
					}
				}
				else if(Flag_Set == 3)
				{
					LED_L ++;
					if(LED_L == 9)LED_L = 1;
					if(LED_L == LED_U)
					{
						LED_L ++;
						if(LED_L == 9)LED_L = 1;
					}
				}
			}
		}
		else if(KeyNum == 4)
		{
			if(Flag_K1 == 1)
			{
				if(Flag_Set == 0)
				{
					Vol_M -= 0.3f;
					if(Vol_M < 0)Vol_M = 3.3f;
				}
				else if(Flag_Set == 1)
				{
					Vol_m -= 0.3f;
					if(Vol_m < 0)Vol_m = 3.3f;
				}
				else if(Flag_Set == 2)
				{
					LED_U --;
					if(LED_U == 0)LED_U = 8;
					if(LED_L == LED_U)
					{
						LED_U --;
						if(LED_U == 0)LED_U = 8;
					}
				}
				else if(Flag_Set == 3)
				{
					LED_L --;
					if(LED_L == 0)LED_L = 8;
					if(LED_L == LED_U)
					{
						LED_L --;
						if(LED_L == 0)LED_L = 8;
					}
				}
			}
		}
		Key_Press_Time = 0;
	}
	
	LCD_Show();
	
	if(getADC_R37() > Vol_M)
	{
		Status = Upper;
		if(Flag_LEDU == 1)LED_ON(LED_U);
		else LED_OFF(LED_U);
	}
	else if(getADC_R37() < Vol_m)
	{
		Status = Lower;
		if(Flag_LEDL == 1)LED_ON(LED_L);
		else LED_OFF(LED_L);
	}
	else if(getADC_R37() >= Vol_m && getADC_R37() <= Vol_M)
	{
		Status = Normal;
		LED_OFF(LED_U);
		LED_OFF(LED_L);
	}
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
	if(Flag_K1 == 0)//数据显示界面
	{
		LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line1, (uint8_t *)"       Main         ");
		LCD_DisplayStringLine(Line2, (uint8_t *)"                    ");
		sprintf((char *) Str, "    Volt:%.2f V     ", getADC_R37());//字符串后面空格个数对ADC有影响
		LCD_DisplayStringLine(Line3, (uint8_t *)Str);
		LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
		if(Status == Upper)
		{
			LCD_DisplayStringLine(Line5, (uint8_t *)"    Status:Upper        ");
		}
		else if(Status == Lower)
		{
			LCD_DisplayStringLine(Line5, (uint8_t *)"    Status:Lower        ");
		}
		else if(Status == Normal)
		{
			LCD_DisplayStringLine(Line5, (uint8_t *)"    Status:Normal       ");
		}
		LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");
	}
	else //参数配置界面
	{
		LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line1, (uint8_t *)"        Setting     ");
		LCD_DisplayStringLine(Line2, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
		LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
		
		if(Flag_Set == 0)
		{
			sprintf((char *) Str, "    Max Volt:%.1fV   ", Vol_M);
			LCD_HighLight(3, Str, 0, 20);//			
			sprintf((char *) Str, "    Min Volt:%.1fV   ", Vol_m);
			LCD_DisplayStringLine(Line5, (uint8_t *)Str);	
			sprintf((char *) Str, "    Upper:LD%d       ", LED_U);
			LCD_DisplayStringLine(Line7, (uint8_t *)Str);	
			sprintf((char *) Str, "    Lower:LD%d       ", LED_L);
			LCD_DisplayStringLine(Line9, (uint8_t *)Str);
		}
		else if(Flag_Set == 1)
		{
			sprintf((char *) Str, "    Max Volt:%.1fV   ", Vol_M);
			LCD_DisplayStringLine(Line3, (uint8_t *)Str);
			sprintf((char *) Str, "    Min Volt:%.1fV   ", Vol_m);
			LCD_HighLight(5, Str, 0, 20);//
			sprintf((char *) Str, "    Upper:LD%d       ", LED_U);
			LCD_DisplayStringLine(Line7, (uint8_t *)Str);
			sprintf((char *) Str, "    Lower:LD%d       ", LED_L);
			LCD_DisplayStringLine(Line9, (uint8_t *)Str);
		}
		else if(Flag_Set == 2)
		{
			sprintf((char *) Str, "    Max Volt:%.1fV   ", Vol_M);
			LCD_DisplayStringLine(Line3, (uint8_t *)Str);
			sprintf((char *) Str, "    Min Volt:%.1fV   ", Vol_m);
			LCD_DisplayStringLine(Line5, (uint8_t *)Str);
			sprintf((char *) Str, "    Upper:LD%d       ", LED_U);
			LCD_HighLight(7, Str, 0, 20);//
			sprintf((char *) Str, "    Lower:LD%d       ", LED_L);
			LCD_DisplayStringLine(Line9, (uint8_t *)Str);
		}
		else if(Flag_Set == 3)
		{
			sprintf((char *) Str, "    Max Volt:%.1fV   ", Vol_M);
			LCD_DisplayStringLine(Line3, (uint8_t *)Str);
			sprintf((char *) Str, "    Min Volt:%.1fV   ", Vol_m);
			LCD_DisplayStringLine(Line5, (uint8_t *)Str);
			sprintf((char *) Str, "    Upper:LD%d       ", LED_U);
			LCD_DisplayStringLine(Line7, (uint8_t *)Str);
			sprintf((char *) Str, "    Lower:LD%d       ", LED_L);
			LCD_HighLight(9, Str, 0, 20);//
		}
	}
}
void LCD_HighLight(uint8_t Line, uint8_t * LCD_Show, uint8_t pos, uint8_t add)
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
