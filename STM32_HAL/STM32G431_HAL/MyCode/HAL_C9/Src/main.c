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
  *这里学到了IIC的正确写法。stop不写， waitACK要写，write要写延时
  *高亮
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_hal.h"
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
	StandBy,
	Setting,
	Running,
	Pause
}TIMERState;
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
uint8_t test_num;

Key_State KeyState = Key_Check;
uint16_t Key_Press_Time = 20, K3_Press_Time = 20;
uint8_t KeyNum;

TIMERState TIMER_State = StandBy;
uint8_t Flag_SetSelect;
uint8_t Hour, Minute, Second;

uint8_t Flag_LED_Flash = 0;

uint8_t ShowTime[20] = "HH:MM:SS";
uint8_t Flag_ROM_Lx = 1;

uint16_t LED_Flash_Time;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HighLight(uint8_t Line, uint8_t * LCD_Show, uint8_t pos, uint8_t add);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_1);

  
  HAL_TIM_Base_Start_IT(&htim6);
  __HAL_TIM_CLEAR_IT(&htim6, TIM_FLAG_UPDATE);
  
  
  Hour = x24C02_Read(0x01);
  Minute = x24C02_Read(0x02);
  Second = x24C02_Read(0x03);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  
	  if(KeyState == Key_Over)
	  {
		  KeyState = Key_Check;
		  if(KeyNum == 1)//选择存储位置1~5
		  {
			  Flag_ROM_Lx ++;
			  if(Flag_ROM_Lx == 6)Flag_ROM_Lx = 1;
			  
			  if(Flag_ROM_Lx == 1)
			  {
				  Hour = x24C02_Read(0x01);
				  Minute = x24C02_Read(0x02);
				  Second = x24C02_Read(0x03);
			  }
			  else if(Flag_ROM_Lx == 2)
			  {
				  Hour = x24C02_Read(0x04);
				  Minute = x24C02_Read(0x05);
				  Second = x24C02_Read(0x06);
			  }	  
			  else if(Flag_ROM_Lx == 3)
			  {
				  Hour = x24C02_Read(0x07);
				  Minute = x24C02_Read(0x08);
				  Second = x24C02_Read(0x09);
			  }
			  else if(Flag_ROM_Lx == 4)
			  {
				  Hour = x24C02_Read(0x0a);
				  Minute = x24C02_Read(0x0b);
				  Second = x24C02_Read(0x0c);
			  }
			  else if(Flag_ROM_Lx == 5)
			  {
				  Hour = x24C02_Read(0x0d);
				  Minute = x24C02_Read(0x0e);
				  Second = x24C02_Read(0x0f);
			  }
		  }
		  else if(KeyNum == 2 && Key_Press_Time < 800)//选择时分�?
		  {
			  TIMER_State = Setting;
			  Flag_SetSelect++;
			  Flag_SetSelect %= 3;
		  }
		  else if(KeyNum == 2 && Key_Press_Time >= 800)
		  {
			  if(TIMER_State == Setting)
			  {
				  if(Flag_ROM_Lx == 1)
				  {
					  x24C02_Write(0x01, Hour);
					  x24C02_Write(0x02, Minute);
					  x24C02_Write(0x03, Second);
				  }
				  else if(Flag_ROM_Lx == 2)
				  {
					  x24C02_Write(0x04, Hour);
					  x24C02_Write(0x05, Minute);
					  x24C02_Write(0x06, Second);
				  }
				  else if(Flag_ROM_Lx == 3)
				  {
					  x24C02_Write(0x07, Hour);
					  x24C02_Write(0x08, Minute);
					  x24C02_Write(0x09, Second);
				  }
				  else if(Flag_ROM_Lx == 4)
				  {
					  x24C02_Write(0x0a, Hour);
					  x24C02_Write(0x0b, Minute);
					  x24C02_Write(0x0c, Second);
				  }
				  else if(Flag_ROM_Lx == 5)
				  {
					  x24C02_Write(0x0d, Hour);
					  x24C02_Write(0x0e, Minute);
					  x24C02_Write(0x0f, Second);
				  }
			  }
			  
			  TIMER_State = StandBy;
		  }
		  else if(KeyNum == 3 && Key_Press_Time < 800)//加一
		  {
			  if(TIMER_State == Setting)
			  {
				  if(Flag_SetSelect == 0){if(++Hour >= 24)Hour = 0;}
				  else if(Flag_SetSelect == 1){if(++Minute >= 60)Minute = 0;}
				  else if(Flag_SetSelect == 2){if(++Second >= 60)Second = 0;}
			  }
		  }
		  else if(KeyNum == 4 && Key_Press_Time < 800)
		  {
			  if(TIMER_State == Running)TIMER_State = Pause;
			  else if(TIMER_State == Pause)TIMER_State = Running;
			  else if(TIMER_State == StandBy)TIMER_State = Running;
			  else if(TIMER_State == Setting)TIMER_State = Running;
		  }
		  else if(KeyNum == 4 && Key_Press_Time >= 800)
		  {
			  TIMER_State = StandBy;
		  }
		  Key_Press_Time = 0;
	  }
	  
	  if(TIMER_State == Running)
	  {
		  if(Flag_LED_Flash == 0)LED_ON(1);
		  else if(Flag_LED_Flash == 1)LED_OFF(1);
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 800);
	  }
	  else if(TIMER_State == Pause || TIMER_State == StandBy)
	  {
		  LED_OFF(1);
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
	  }
	  
	  if(Flag_ROM_Lx == 1)LCD_DisplayStringLine(Line2, (uint8_t *)"    NO 1");
	  else if(Flag_ROM_Lx == 2)LCD_DisplayStringLine(Line2, (uint8_t *)"    NO 2");
	  else if(Flag_ROM_Lx == 3)LCD_DisplayStringLine(Line2, (uint8_t *)"    NO 3");
	  else if(Flag_ROM_Lx == 4)LCD_DisplayStringLine(Line2, (uint8_t *)"    NO 4");
	  else if(Flag_ROM_Lx == 5)LCD_DisplayStringLine(Line2, (uint8_t *)"    NO 5");
	  
	  if(TIMER_State == StandBy)LCD_DisplayStringLine(Line8, (uint8_t *)"      StandBy");
	  else if(TIMER_State == Pause)LCD_DisplayStringLine(Line8, (uint8_t *)"      Pause");
	  else if(TIMER_State == Running)LCD_DisplayStringLine(Line8, (uint8_t *)"      Running");
	  else if(TIMER_State == Setting)LCD_DisplayStringLine(Line8, (uint8_t *)"      Setting");
	
	  sprintf((char *)ShowTime, "     %2d:%2d:%2d       ", Hour, Minute, Second);
//	  LCD_DisplayStringLine(Line6, ShowTime);
	  if(Flag_SetSelect == 0)HighLight(7, ShowTime, 5, 3);
	  else if(Flag_SetSelect == 1)HighLight(7, ShowTime, 8, 3);
	  else if(Flag_SetSelect == 2)HighLight(7, ShowTime, 11, 3);
	  
	  
	  
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
}

/* USER CODE BEGIN 4 */
void HighLight(uint8_t Line, uint8_t * LCD_Show, uint8_t pos, uint8_t add)
{
	uint8_t i;
	
	for(i = 0; i < 20; i++)
	{
		if(i != pos)LCD_DisplayChar(24 * Line, 320 - (16 * i), LCD_Show[i]);
		else if(i == pos)i += add;
	}
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	
	for(i = 0; i < add; i++)
	{
		
		LCD_DisplayChar(24 * Line, 320 - (16 * (pos + i)), LCD_Show[pos + i]);
	}
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t Tcount;
	if(++Tcount == 1000)
	{
		Tcount = 0;
		if(TIMER_State == Running)
		{
			Second --;
			if(Second == 255)
			{
				Second = 59;
				Minute --;
				if(Minute == 255)
				{
					Minute = 59;
					Hour --;
					if(Hour == 255)
					{
						Hour = 0;
						Minute = 0;
						Second = 0;
						TIMER_State = StandBy;//计时结束
					}
				}
			}
		}
	}
	
	if(++LED_Flash_Time == 500)
	{
	  LED_Flash_Time = 0;
	  Flag_LED_Flash = !Flag_LED_Flash;
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
				if(K3 == Press)
				{
					K3_Press_Time += 10;
					if(K3_Press_Time >= 800)
					{
						if(Flag_SetSelect == 0){if(++Hour == 24)Hour = 0;}
						else if(Flag_SetSelect == 1){if(++Minute == 60)Minute = 0;}
						else if(Flag_SetSelect == 2){if(++Second == 60)Second = 0;}
					}
				}
			}
			else
			{
				KeyState = Key_Over;
				K3_Press_Time = 0;
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
