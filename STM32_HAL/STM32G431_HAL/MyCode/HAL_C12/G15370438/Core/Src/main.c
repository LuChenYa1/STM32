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
#include <string.h>
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
struct a
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Str[20];
uint8_t RX[22] = "XNBR:XXXX:YYMMDDHHmmSS";
Key_State KeyState = Key_Check;
uint8_t KeyNum;
uint16_t Key_Press_Time = 20;
uint8_t ShowSelect, Flag_K4;
uint16_t Count_CNBR, Count_VNBR, IDLE = 8;
float Fee_CNBR = 3.5f, Fee_VNBR = 2.0f;
uint8_t  Car[8][4], Car_Count, i, j, Flag_Break;
struct a info_In[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(RX[5 + j] == Car[i][j])
			{
				if(j == 3)
				{
					//出停车场
					Car_Count--;
					IDLE ++;
					Flag_Break = 1;
					
					if(RX[0] == 'V')
					{
						Count_VNBR --;
					}
					else if(RX[0] == 'C')
					{
						Count_CNBR --;
					}
					Car[i][j] = ' ';
					Car[i][j - 1] = ' ';
					Car[i][j - 2] = ' ';
					break;
				}
				continue;
			}
			if(i == 7 && j == 3)
			{
				//进停车场,新车
				IDLE --;
				Car_Count++;
				
				if(RX[0] == 'V')
				{
					Count_VNBR ++;
				}
				else if(RX[0] == 'C')
				{
					Count_CNBR ++;
				}
				Car[Car_Count - 1][0] = RX[5];
				Car[Car_Count - 1][1] = RX[6];
				Car[Car_Count - 1][2] = RX[7];
				Car[Car_Count - 1][3] = RX[8];
				
				info_In[Car_Count - 1].Year = (RX[10] - 48) * 10 + (RX[11] - 48);//存时间
				info_In[Car_Count - 1].Month = (RX[12] - 48) * 10 + (RX[13] - 48);
				info_In[Car_Count - 1].Date = (RX[14] - 48) * 10 + (RX[15] - 48);
				info_In[Car_Count - 1].Hour = (RX[16] - 48) * 10 + (RX[17] - 48);
				info_In[Car_Count - 1].Minute = (RX[18] - 48) * 10 + (RX[19] - 48);
				info_In[Car_Count - 1].Second = (RX[20] - 48) * 10 + (RX[21] - 48);
			}
		}
		if(Flag_Break == 1)
		{
			Flag_Break = 0;
			break;
		}
	}
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)RX, 22);
	HAL_UART_Receive_IT(&huart1, RX, 22);
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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_UART_Receive_IT(&huart1, RX, 22);
  
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_2);
  __HAL_TIM_SetAutoreload(&htim3, 500);//2KHZ
  
 
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
		  }
		  else if(KeyNum == 2)
		  {
			  if(ShowSelect == 1)
			  {
				  Fee_CNBR += 0.5f;
				  Fee_VNBR += 0.5f;
			  }
		  }
		  else if(KeyNum == 3)
		  {
			  if(ShowSelect == 1)
			  {
				  Fee_CNBR -= 0.5f;
				  Fee_VNBR -= 0.5f;
			  }
		  }
		  else if(KeyNum == 4)
		  {
			  Flag_K4 = !Flag_K4;
			  if(Flag_K4 == 0)
			  {
				  LED_OFF(2);
				  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
			  }
			  else
			  {
				  LED_ON(2);
				  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);
			  }
		  }
		  Key_Press_Time = 0;
	  }
	  
	sprintf((char *)Str, "Year:%d", info[0].Year);
	LCD_DisplayStringLine(Line8, (uint8_t *)Str);
	  
	if(ShowSelect == 0)//车位显示界面
	{
		LCD_DisplayStringLine(Line1, (uint8_t *)"       Data");
		sprintf((char *)Str, "   CNBR:%d", Count_CNBR);
		LCD_DisplayStringLine(Line3, (uint8_t *)Str);
		sprintf((char *)Str, "   VNBR:%d", Count_VNBR);
		LCD_DisplayStringLine(Line5, (uint8_t *)Str);
		sprintf((char *)Str, "   IDLE:%d", IDLE);
		LCD_DisplayStringLine(Line7, (uint8_t *)Str);
	}
	else if(ShowSelect == 1)//费率设置界面
	{
		LCD_DisplayStringLine(Line1, (uint8_t *)"       Para");
		sprintf((char *)Str, "   CNBR:%.2f", Fee_CNBR);
		LCD_DisplayStringLine(Line3, (uint8_t *)Str);
		sprintf((char *)Str, "   VNBR:%.2f", Fee_VNBR);
		LCD_DisplayStringLine(Line5, (uint8_t *)Str);
		LCD_DisplayStringLine(Line7, (uint8_t *)"                  ");
	}
	
	if(IDLE != 0)LED_ON(1);
	else LED_OFF(1);
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
