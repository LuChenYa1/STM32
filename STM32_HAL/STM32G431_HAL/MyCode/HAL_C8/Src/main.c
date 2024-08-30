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
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_hal.h"
#include <stdio.h>
#include <string.h>
#include "LED.h"
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
	DU_NotClear,
	close, 
	open
}State_f;
typedef enum
{
	up,
	down,
}DownAndUp;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define B1 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
#define B2 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define B3 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define B4 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define Press   GPIO_PIN_RESET
#define Release GPIO_PIN_SET

#define UP_PA4    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define DOWN_PA4  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define OPEN_PA5  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define CLOSE_PA5 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define UP_PWM    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 800)
#define	DOWN_PWM  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 600);  
#define OPEN_PWM  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 600);
#define	CLOSE_PWM __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 500);  
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t ShowTime[10] = "HH:MM:SS";
uint8_t ShowDate[10] = "YYYY:MM:DD";
uint8_t Str[30];
Key_State KeyState = Key_Check;
uint16_t Key_Press_Time;
uint8_t KeyNum;

uint8_t Plot_Now = 1;
uint8_t Plot_Aim[5] = {0};

extern uint8_t LCD_Show_Flag;

uint8_t Flag_Key_Not, Flag_Work;
uint32_t Key_Not_Time;

State_f State = close;
DownAndUp DAU = up;

extern uint8_t LED_Num;
uint8_t LED_Flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
void Plot_AIM_Show(void);
void Close_Door(void);
void Open_Door(void);
void Down(void);
void Up(void);
void LED_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void RTC_Show(uint8_t *ShowTime, uint8_t *ShowDate)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	sprintf((char *)ShowTime, "      %2d:%2d:%2d", sTime.Hours, sTime.Minutes, sTime.Seconds);
//	sprintf((char *)ShowDate, "%2d-%2d-%2d", 2000 + sDate.Year, sDate.Month, sDate.Date);
	LCD_DisplayStringLine(Line8, ShowTime);
}

void RTC_SetTime(uint8_t HH, uint8_t MM, uint8_t SS)
{
	RTC_TimeTypeDef sTime = {0};
	
	sTime.Hours = HH;
	sTime.Minutes = MM;
	sTime.Seconds = SS;
	
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
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
  MX_RTC_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();//灯初始化
  
  LCD_Init();//LCD初始�?
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line2, (uint8_t *)"    Now - Platfom");
  
  RTC_SetTime(13, 0, 0);//RTC初始�?
  
  HAL_TIM_Base_Start_IT(&htim6);//TIM6_Base
  __HAL_TIM_CLEAR_IT(&htim6, TIM_FLAG_UPDATE);
  
  HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);//PWM初始�?
  __HAL_TIM_CLEAR_IT(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_IT(&htim16, TIM_CHANNEL_1);
  __HAL_TIM_CLEAR_IT(&htim16, TIM_CHANNEL_1);
  
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  RTC_Show(ShowTime, ShowDate);
	  
	  if(KeyState == Key_Over)//按键判断
	  {
		  KeyState = Key_Check;
		  if(State != DU_NotClear)//上下行时，按键无�?
		  {
			  if(KeyNum == 1)
			  {
				  Plot_Aim[1] = 1;  
			  }
			  else if(KeyNum == 2)
			  {
				  Plot_Aim[2] = 1;
			  }
			  else if(KeyNum == 3)
			  {
				  Plot_Aim[3] = 1;
			  }
			  else if(KeyNum == 4)
			  {
				  Plot_Aim[4] = 1;
			  }
		  }
		  
		  Key_Press_Time = 20;
	  }
	  
	  Plot_AIM_Show();//LED_Aim
	  LED_Show();//Up Down流水�?


	  if(Plot_Aim[Plot_Now] == 1)//LCD闪烁
	  {
		  if(LCD_Show_Flag == 1)
		  {
			  sprintf((char *) Str, "          %d", Plot_Now);//实时刷新当前�?在平�?
			  LCD_DisplayStringLine(Line4, (uint8_t *)Str);
		  }
		  else {LCD_DisplayStringLine(Line4, (uint8_t *)"               ");} 
	  }
	  else 
	  {
		  sprintf((char *) Str, "          %d", Plot_Now);
		  LCD_DisplayStringLine(Line4, (uint8_t *)Str);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	static uint16_t Tcount1 = 0, Tcount2 = 0;
	
	if(Flag_Key_Not == 1)Key_Not_Time ++;
	if(Key_Not_Time >= 1000)Flag_Work = 1;//松手�?秒后�?始运�?
	
	if(Flag_Work == 1)//Run�?始运�?
	{
		if(State == close)//关门状�??
		{
			if(++Tcount1 <= 4000)//4s
			{
				Close_Door();
			}
			else 
			{
				Tcount1 = 0;
				State = DU_NotClear;
			}
		}
		
		else if(State == DU_NotClear)//上下�?
		{
			if(DAU == up)//上行
			{
				if(Plot_Now < 4)
				{
					if(++Tcount1 <= 6000)//6s
					{
						Up();
					}
					else
					{
						Tcount1 = 0;
						Plot_Now ++;//当前平台+1
						if(Plot_Aim[Plot_Now] == 1)State = open;
					}
				}
				else
				{
					DAU = down;//已到�?高层，开始下�?
				}
			}
			else if(DAU == down)//下行
			{
				if(Plot_Now > 1)
				{
					if(++Tcount1 <= 6000)//4
					{
						Down();
					}
					else
					{
						Tcount1 = 0;
						Plot_Now --;
						if(Plot_Aim[Plot_Now] == 1)State = open;
					}
				}
				else 
				{
					DAU = up;//已到�?低层，开始上�?
				}
			}
		}			
		else if(State == open)//�?�?,即为目标平台
		{
			if(++Tcount1 <= 4000)//4
			{
				Open_Door();
			}
			else
			{
				if( ++Tcount2 == 2000)//停留两秒
				{
					Tcount2 = 0;
					Tcount1 = 0;
					Plot_Aim[Plot_Now] = 0;//目标平台失效
					State = close;//关门
				}
			}
		}
	 }
}
void LED_Show(void)//流水�?
{
	if(DAU == up)//上行
	{
		if(LED_Flag == 0)
		{
			LED_Num = 5;//保证从最右开始流�?
			LED_Flag = 1;
		}
		LED_ON(LED_Num);
		if(LED_Num > 5)LED_OFF(LED_Num - 1);
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == RESET)LED_OFF(8);//前面的灯�?
	}	
	if(DAU == down)//下行
	{
		if(LED_Flag == 1)
		{
			LED_Num = 5;//保证从最左开始流�?
			LED_Flag = 0;
		}
		LED_ON(13 - LED_Num);
		if(LED_Num > 5)LED_OFF((13 - LED_Num) + 1);
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == RESET)LED_OFF(5);//前面的灯�?
	}	
}


void Plot_AIM_Show(void)//目标平台显示�?
{
	  if(Plot_Aim[1] == 1)//目标=1
	  {
		  if(Plot_Now != 1)//目标不等于Now,则正常亮�?
		  {
			 LED_ON(1);
		  }
		  else {LED_OFF(1);}  
	  }
	  if(Plot_Aim[2] == 1)
	  {
		  if(Plot_Now != 2)
		  {
			 LED_ON(2);
		  }
		  else {LED_OFF(2);}
	  }
	  if(Plot_Aim[3] == 1)
	  {
		  if(Plot_Now != 3)
		  {
			 LED_ON(3);
		  }
		  else {LED_OFF(3);}
	  }
	  if(Plot_Aim[4] == 1)
	  {
		  if(Plot_Now != 4)
		  {
			 LED_ON(4);
		  }
		  else{LED_OFF(4);}
	  }
}

void Close_Door(void)
{
	CLOSE_PWM;//关门
	CLOSE_PA5;
}
void Open_Door(void)
{
	OPEN_PWM;//�?�?
	OPEN_PA5;
}		
void Down(void)
{
	DOWN_PWM;//下行
	DOWN_PA4;
}
void Up(void)
{
	UP_PWM;//上行
	UP_PA4;
}
		
		
void Key_Scan(void)
{
	switch(KeyState)
	{
		case Key_Check:
		{
			if(B1 == Press || B2 == Press || B3 == Press || B4 == Press)
			{
				KeyState = Key_Press;
				Flag_Key_Not = 0;
			}
		}
		break;
		case Key_Press:
		{
			if(B1 == Press)KeyNum = 1;
			else if(B2 == Press)KeyNum = 2;
			else if(B3 == Press)KeyNum = 3;
			else if(B4 == Press)KeyNum = 4;
			KeyState = Key_Release;
		}
		break;
		case Key_Release:
		{
			if(B1 == Press || B2 == Press || B3 == Press || B4 == Press)
			{
				Key_Press_Time += 10;
			}
			else 
			{
				KeyState = Key_Over;
				Flag_Key_Not = 1;
				Key_Not_Time = 0;
			}
		}
		break;
		default:break;
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
