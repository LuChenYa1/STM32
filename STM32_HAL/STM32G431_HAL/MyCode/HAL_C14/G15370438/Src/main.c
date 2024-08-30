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
#include "lcd.h"
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
Key_State KeyState = Key_Check;
uint8_t KeyNum;
uint16_t Key_Press_Time = 20;
uint8_t Str[20];
float Duty, Volt, V, V_MH, V_ML;
uint8_t ShowSelect = 0, Mode_PWM, R = 1, R_Temp = 1, K = 1, K_Temp = 1, N_PWM = 0, RK_Select, LOCK_Duty = 0, Flag_K2_PWM;
uint16_t AutoReload = 500, Freq = 4000, Freq_IC_Value, Freq_IC;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float getADC_R37(void);
void LCD_Show(void);

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef * htim)
{
	if(htim ->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Freq_IC_Value = __HAL_TIM_GET_COUNTER(&htim3);
			__HAL_TIM_SetCounter(&htim3, 0);
			Freq_IC = 1000000 / Freq_IC_Value;
			HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
  __HAL_TIM_CLEAR_IT(&htim2, TIM_CHANNEL_2);
  Duty = 50;//50%
  
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Volt = getADC_R37();
	  sprintf((char *)Str, "     %.2f", Volt);
      LCD_DisplayStringLine(Line6, Str);
	  
	  if(KeyState == Key_Over)
	  {
		  KeyState = Key_Check;
		  if(KeyNum == 1)
		  {
			  ShowSelect ++;
			  if(ShowSelect == 1)RK_Select = 0;
			  else if(ShowSelect == 2)
			  {
				  R = R_Temp;
				  K = K_Temp;
			  }
			  ShowSelect %= 3;
		  }
		  else if(KeyNum == 2)
		  {
			  if(ShowSelect == 0 && Flag_K2_PWM == 0)
			  {
				  Mode_PWM = !Mode_PWM;
				  N_PWM++;
				  Flag_K2_PWM = 1;//5000ms
				  LOCK_Duty = 1;
			  }
			  else if(ShowSelect == 1)
			  {
				  RK_Select = !RK_Select;
			  }
		  }
		  else if(KeyNum == 3)
		  {
			  if(ShowSelect == 1)
			  {
				  if(RK_Select == 0)
				  {
					  R_Temp++;
					  if(R_Temp == 11)R_Temp = 1;
				  }
				  else if(RK_Select == 1)
				  {
					  K_Temp++;
					  if(K_Temp == 11)K_Temp = 1;
				  }
			  }
		  }
		  else if(KeyNum == 4 && Key_Press_Time < 2000)
		  {
			  if(ShowSelect == 1)
			  {
				  if(RK_Select == 0)
				  {
					  R_Temp--;
					  if(R_Temp == 0)R_Temp = 10;
				  }
				  else if(RK_Select == 1)
				  {
					  K_Temp--;
					  if(K_Temp == 0)K_Temp = 10;
				  }
			  }
			  if(ShowSelect == 0)
			  {
				  if(LOCK_Duty == 1)LOCK_Duty = 0;//解锁
			  }
		  }
		  else if(KeyNum == 4 && Key_Press_Time >= 2000)
		  {
		      if(ShowSelect == 0)
			  {
				  LOCK_Duty = 1;
			  } 
		  }
		  Key_Press_Time = 0;
	  }
	  
	  if(LOCK_Duty == 0)
	  {
		  
		  if(Volt < 1.0f)
		  {
			  Duty = 10.0f;//10%
		  }
		  else if(Volt < 3.0f && Volt > 1.0f)
		  {
			  Duty = 37.5f * Volt - 27.5f;
		  }
		  else
		  {
			  Duty = 85.0f;
		  }
	  }
	  
	  
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Duty / 100.0f  * AutoReload);//
//	  htim3.Instance->CCR2 = ...输出比较�?
	  __HAL_TIM_SetAutoreload(&htim2, AutoReload);
//	  htim3.Instance->ARR = ...自动重装载�??
	  	  
	  V = Freq_IC * 2 * 3.14f * R / (100.0f * K);  
	  V_MH = 8000 * 2 * 3.14f * R / (100.0f * K); 
	  V_ML = 4000 * 2 * 3.14f * R / (100.0f * K); 
	  
	  if(ShowSelect == 0)LED_ON(1);
	  else LED_OFF(1);
	  
	  if(LOCK_Duty == 1)LED_ON(3);
	  else LED_OFF(3);
	  
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
void LCD_Show(void)
{
	if(ShowSelect == 0)//数据界面
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"        DATA");
		  if(Mode_PWM == 0 && Freq == 4000)
		  {
			  sprintf((char *)Str, "     M=L");
			  LCD_DisplayStringLine(Line3, Str);
		  }
		  else if(Mode_PWM == 1&& Freq == 8000)
		  {
			  sprintf((char *)Str, "     M=H   ");
			  LCD_DisplayStringLine(Line3, Str);
		  }
		  sprintf((char *)Str, "     P=%.0f%%  ", Duty);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "     V=%.1f    ", V);
		  LCD_DisplayStringLine(Line5, Str);
	  }
	  else if(ShowSelect == 1)//参数界面
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"        PARA");
		  sprintf((char *)Str, "     R=%d   ", R_Temp);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "     K=%d   ", K_Temp);
		  LCD_DisplayStringLine(Line4, Str);
		  LCD_DisplayStringLine(Line5, (uint8_t *)"               ");
	  }
	  else if(ShowSelect == 2)//统计界面
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"        RECD");
		  sprintf((char *)Str, "     N=%d   ", N_PWM);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "     MH=%.1f    ", V_MH);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "     ML=%.1f    ", V_ML);
		  LCD_DisplayStringLine(Line5, Str);
	  }
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

float getADC_R37(void)
{
	uint16_t temp;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	temp = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return (temp * 3.3f / 4096);
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
