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
#include "tim.h"
#include "usart.h"
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

#define Press   GPIO_PIN_RESET
#define Release GPIO_PIN_SET
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RX[7], Str[20];
uint8_t KeyNum;
uint32_t LED_Start_Time, Key_Press_Time = 20, LED2_Start_Time, LED2_Last_Time;
Key_State KeyState = Key_Check;
uint16_t B1 = '@', B2 = '@', B3 = '@', ShowSelect;
uint16_t Freq, Duty;
uint16_t Code_Key = 0, Code_UART = 123;
uint8_t Flag_5s;
uint8_t Flag_PWM_LED, Error_Count;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
void LCD_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)RX, 7);
	if((Code_UART / 100) == RX[0] - 48 && (Code_UART / 10 % 10) == RX[1] - 48 && (Code_UART % 10) == RX[2] - 48 )//当前密码正确
	{	
		Code_UART = (uint8_t)(RX[4] - 48) * 100 + (uint8_t)(RX[5] - 48) * 10 + (uint8_t)(RX[6] - 48);
	}
	HAL_UART_Receive_IT(&huart1, RX, 7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	if(Flag_PWM_LED == 1)
    {
	    if(++LED_Start_Time == 5000)
	    {
		    LED_Start_Time = 0;
		    Flag_5s = 1;
			Flag_PWM_LED = 0;
	    }
	}
	if(Error_Count == 3)
	{
		if(++LED2_Start_Time == 100)
		{
			LED2_Start_Time = 0;
			LED_TRG(2);
		}
		if(++LED2_Last_Time == 5000)
		{
			LED2_Last_Time = 0;
			Error_Count = 0;
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_UART_Receive_IT(&huart1, RX, 7);
  
  HAL_TIM_Base_Start_IT(&htim6);
   __HAL_TIM_CLEAR_IT(&htim6, TIM_FLAG_UPDATE);
   
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
  __HAL_TIM_CLEAR_IT(&htim2, TIM_CHANNEL_2);
  
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
			  if(ShowSelect == 0)
			  {
				  if(B1 == '@')B1 = 0;
				  else 
				  {
					  B1 ++;
					  if(B1 >= 10)B1 = 0;
				  }
			  }
		  }
		  else if(KeyNum == 2)
		  {
			  if(ShowSelect == 0)
			  {
				  if(B2 == '@')B2 = 0;
				  else 
				  {
					  B2 ++;
					  if(B2 >= 10)B2 = 0;
				  }
			  }
		  }
		  else if(KeyNum == 3)
		  {
			  if(ShowSelect == 0)
			  {
				  if(B3 == '@')B3 = 0;
				  else 
				  {
					  B3 ++;
					  if(B3 >= 10)B3 = 0;
				  }
			  }
		  }
		  else if(KeyNum == 4)
		  {
			  if(ShowSelect == 0)
			  {
				  Code_Key = 0;
				  if(B1 == 0)Code_Key = B2 * 10 + B3;
				  else  Code_Key = B1 * 100 + B2 * 10 + B3;
				  if(Code_Key != Code_UART)
				  {
					  Error_Count++;
					  if(Error_Count == 4)Error_Count = 1;
					  
					  B1 = '@';B2 = '@';B3 = '@';
				  }
			  }
		  }
		  Key_Press_Time = 0;
	  }
	  
	  if(Code_Key != Code_UART)
	  {
		  __HAL_TIM_PRESCALER(&htim2, 79);
		  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
	  }
	  else//密码相同�?
	  {
		  if(ShowSelect == 0)
		  {
			  ShowSelect = 1;
		  }
		  if(Flag_5s == 1)
		  {
			  LED_OFF(1);
			  Code_Key = 0;
			  __HAL_TIM_PRESCALER(&htim2, 79);
			  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
			  Duty = 50;
			  Freq = 1000;
			  ShowSelect = 0;
			  B1 = '@';B2 = '@';B3 = '@';
			  Flag_5s = 0;
		  }  
		  else
		  {
			  Flag_PWM_LED = 1;
			  LED_ON(1);
			  __HAL_TIM_PRESCALER(&htim2, 39);
			  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 100);
			  Duty = 10;
			  Freq = 2000;
		  }
	  }
	  
	  LCD_Show();
	  sprintf((char *)Str, "Code_Key:%d ", Code_Key);
	  LCD_DisplayStringLine(Line8, Str);
	  sprintf((char *)Str, "Code_UART:%d", Code_UART);
	  LCD_DisplayStringLine(Line9, Str);
	  
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void LCD_Show(void)
{
  if(ShowSelect == 0)//密码输入界面
  {
	  LCD_DisplayStringLine(Line1, (uint8_t *)"       PSD");
	  
	  if(B1 == '@')
	  {
		  sprintf((char *)Str, "    B1:%c     ", B1);
		  LCD_DisplayStringLine(Line3, Str);
	  }
	  else
	  {
		  sprintf((char *)Str, "    B1:%1d     ", B1);
		  LCD_DisplayStringLine(Line3, Str);
	  }
	  if(B2 == '@')
	  {
		  sprintf((char *)Str, "    B2:%c      ", B2);
		  LCD_DisplayStringLine(Line4, Str);
	  }
	  else 
	  {
		  sprintf((char *)Str, "    B2:%1d      ", B2);
		  LCD_DisplayStringLine(Line4, Str);
	  }
	  if(B3 == '@')
	  {
		  sprintf((char *)Str, "    B3:%c       ", B3);
		  LCD_DisplayStringLine(Line5, Str);
	  }
	  else
	  {
		  sprintf((char *)Str, "    B3:%1d      ", B3);
		  LCD_DisplayStringLine(Line5, Str);
	  } 
  }
  else//输出状�?�界�?
  {
	  LCD_DisplayStringLine(Line1, (uint8_t *)"       STA");
	  sprintf((char *)Str, "    F:%dHZ        ", Freq);
	  LCD_DisplayStringLine(Line3, Str);
	  sprintf((char *)Str, "    D:%2d%%       ", Duty);
	  LCD_DisplayStringLine(Line4, Str);
	  LCD_DisplayStringLine(Line5, (uint8_t *)"           ");
  }
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
