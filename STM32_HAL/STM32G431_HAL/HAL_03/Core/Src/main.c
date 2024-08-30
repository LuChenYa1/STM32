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
#include <string.h>
#include <stdio.h>
#include "ws2812b.h"
//#include "lcd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define K210    HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define Have    GPIO_PIN_SET   
#define NotHave GPIO_PIN_RESET 

#define PUL_Port GPIOB
#define PUL_Pin  GPIO_PIN_13
#define DIR_Port GPIOB
#define DIR_Pin  GPIO_PIN_14
#define PUL_TRG HAL_GPIO_TogglePin(PUL_Port, PUL_Pin)
#define PUL_H   HAL_GPIO_WritePin(PUL_Port, PUL_Pin, GPIO_PIN_SET)
#define PUL_L   HAL_GPIO_WritePin(PUL_Port, PUL_Pin, GPIO_PIN_RESET)

#define DIR_TRG HAL_GPIO_TogglePin(DIR_Port, DIR_Pin)
#define DIR_H   HAL_GPIO_WritePin(DIR_Port, DIR_Pin, GPIO_PIN_SET)
#define DIR_L   HAL_GPIO_WritePin(DIR_Port, DIR_Pin, GPIO_PIN_RESET)

#define Trig_Port GPIOA
#define Trig_Pin  GPIO_PIN_6
#define TRIG_ON   HAL_GPIO_WritePin(Trig_Port, Trig_Pin, GPIO_PIN_SET)
#define TRIG_OFF  HAL_GPIO_WritePin(Trig_Port, Trig_Pin, GPIO_PIN_RESET)

#define SR04_Min  46
#define SR04_Max  30
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t Ema_Count, Stop_Time_Count, Check_Time, Have_Count;
uint8_t Flag_Check, Flag_Run, Flag_Down, Flag_Mid = 0, Flag_Up = 0, Flag_MidUpMid = 0, Flag_Act, Flag_Percent;
uint8_t Str[20], Str_Esp8266[20];
double Light;
uint8_t Time_Sound_Interrupt, Angle = 0;//0-180

uint8_t Temperature = 27, Humidity = 59;
uint16_t Percent_Rubbish;//0-100
uint8_t RX[50];

uint8_t  Distance = 0, array[50];
uint8_t  Channel2Edge = 0;
uint16_t Channel2HighTime, Channel2RisingTime, Channel2FallingTime;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
double getADC_PA5(void);
void LED_Show(void);
void gases_start_signal(void);
void LCD_Show(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{
//	if(htim->Instance == htim6.Instance)
//	{
//		//上电进入工作状态（平铺）后才能进行K210检测
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
//		
//		//K210执行逻辑
//		if(Flag_Check == 1)//开始监测K210
//		{
//			if(K210 == Have)Have_Count ++;
//			if(++Check_Time == 2000)
//			{
//				if((Have_Count * 1.0f / Check_Time) > 0.4f)
//				{
//					Flag_Run = 1;//大部分时候都是有垃圾,触发花瓣合拢
//					Flag_Check = 0;//停止监测k210
//					LED_ON(2);
//					LED_OFF(1);
//				}
//				Have_Count = 0;
//				Check_Time = 0;
//			}
//		}
//		
//		if(Flag_Run == 1)//开始向上合拢
//		{
//			if(Angle == 90)
//			{
//				DIR_H;
//				PUL_TRG;
//				if(++Ema_Count >= 1550)
//				{
//					Ema_Count = 0;	
//					Flag_Run = 2;
//					LED_ON(3);
//					LED_OFF(2);
//					Angle += 90;
//				}
//			}
//		}
//		else if(Flag_Run == 2)//合拢完毕，暂停1s
//		{
//			DIR_L;
//			if(++Stop_Time_Count == 1000)
//			{
//				Stop_Time_Count = 0;	
//				Flag_Run = 3;
//				LED_ON(4);
//				LED_OFF(3);
//			}
//		}
//		else if(Flag_Run == 3)//合拢完毕，向下收回平铺
//		{
//			if(Angle == 180)
//			{
//				DIR_L;
//				PUL_TRG;
//				if(++Ema_Count == 1550)
//				{
//					Ema_Count = 0;	
//					Flag_Check = 1;//重新开始监测K210
//					Flag_Run = 0;//停止运行合拢程序
//					Angle -= 90;
//				}
//			}
//		}
//		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
//		//语音指令执行逻辑
//		if(Flag_Down == 1 && Flag_Percent == 0)//从任意状态垂落
//		{
//			Flag_Mid = 0;Flag_Up = 0;Flag_MidUpMid = 0;//垂落优先级最高
//			Flag_Check = 0;//不是平铺，不能检测K210
//			LED_OFF(6);LED_OFF(7);LED_OFF(8);
//			if(Angle == 90)
//			{
//				DIR_L;
//				PUL_TRG;
//				if(++Ema_Count >= 1600)
//				{
//					Ema_Count = 0;
//					Flag_Down = 0;
//					Angle -= 90;
//					LED_OFF(5);
//				}
//			}
//			else if(Angle == 180)
//			{
//				DIR_L;
//				PUL_TRG;
//				if(++Ema_Count >= 3150)
//				{
//					Ema_Count = 0;
//					Flag_Down = 0;
//					Angle -= 180;
//					LED_OFF(5);
//				}	
//			}
//			else if(Angle == 0)
//			{
//				DIR_H;
//				Ema_Count = 0;
//				Flag_Down = 0;
//				LED_OFF(5);
//			}
//		}
//		
//		if(Flag_Mid == 1 && Flag_Percent == 0)//从任意状态平铺
//		{
//			Flag_Up = 0;Flag_MidUpMid = 0;//次高优先级
//			LED_OFF(7);LED_OFF(8);
//			Flag_Check = 0;//没到平铺状态，不能检测K210
//			if(Angle == 0)
//			{
//				DIR_H;
//				PUL_TRG;
//				if(++Ema_Count == 1600)
//				{
//					Ema_Count = 0;	
//					Flag_Mid = 0;
//					Flag_Check = 1;//到平铺状态，可以开始监测K210
//					Angle += 90;
//					LED_OFF(6);
//				}
//			}
//			else if(Angle == 180)
//			{
//				DIR_L;
//				PUL_TRG;
//				if(++Ema_Count == 1550)
//				{
//					Ema_Count = 0;	
//					Flag_Mid = 0;
//					Flag_Check = 1;
//					Angle -= 90;
//					LED_OFF(6);
//				}
//			}
//			else if(Angle == 90)
//			{
//				Flag_Mid = 0;
//				Flag_Check = 1;
//				Ema_Count = 0;
//				LED_OFF(6);
//			}
//		}
//		
//		if(Flag_Up == 1)//从任意状态合拢
//		{
//			Flag_MidUpMid = 0;//第三优先级
//			LED_OFF(8);
//			Flag_Check = 0;//不是平铺，不能检测K210
//			if(Angle == 90)
//			{
//				DIR_H;
//				PUL_TRG;
//				if(++Ema_Count >= 1550)
//				{
//					Ema_Count = 0;	
//					Flag_Up = 0;
//					Angle += 90;
//					LED_OFF(7);
//				}
//			}
//			else if(Angle == 0)
//			{
//				Flag_Check = 0;
//				DIR_H;
//				PUL_TRG;
//				if(++Ema_Count >= 3150)
//				{
//					Ema_Count = 0;	
//					Flag_Up = 0;
//					Angle += 180;
//					LED_OFF(7);
//				}
//			}
//			else if(Angle == 180)
//			{
//				DIR_L;
//				Flag_Up = 0;
//				Ema_Count = 0;	
//				LED_OFF(7);
//			}
//		}
//		
//		if(Flag_MidUpMid == 1  && Flag_Percent == 0)//收集垃圾
//		{
//			Flag_Check = 0;//不是平铺，不能检测K210
//			if(Angle == 90)
//			{
//				DIR_H;
//				PUL_TRG;
//				if(++Ema_Count >= 1550)
//				{
//					Ema_Count = 0;
//					Angle += 90;
//				}
//			}
//			else if(Angle == 180)
//			{
//				if(Flag_Act == 0)
//				{
//					DIR_L;
//					if(++Stop_Time_Count == 1000)
//					{
//						Stop_Time_Count = 0;	
//						Flag_Act = 1;
//					}
//				}
//				else 
//				{
//					DIR_L;
//					PUL_TRG;
//					if(++Ema_Count >= 1550)
//					{
//						Ema_Count = 0;
//						Angle -= 90;
//						Flag_MidUpMid = 0;
//						Flag_Act = 0;
//						Flag_Check = 1;//回到平铺状态，重新开启K210监测
//						LED_OFF(8);
//					}
//				}
//			}
//		}
//		
//	}
//	if(htim->Instance == htim2.Instance)
//	{
//		Time_Sound_Interrupt ++;
//	}
}

//串口接收处理
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{		
//	if(huart->Instance == huart2.Instance)//语音
//	{
//		if(RX[0]=='1')
//		{
//			LED_ON(5);
//			Flag_Down = 1;
//		}
//		else if(RX[0]=='2')
//		{
//			LED_ON(8);
//			Flag_MidUpMid = 1;
//		}
//		else if(RX[0]=='3')
//		{
//			LED_ON(6);
//			Flag_Mid = 1;
//			LED_ON(1);
//		}
//		else if(RX[0]=='4')
//		{
//			LED_ON(7);
//			Flag_Up = 1;
//		}	
//		else if(RX[0]=='0')
//		{
//			
//		}
//		HAL_UART_Receive_IT(&huart2, (uint8_t *)RX,1);
//	}
//	
//	if(huart->Instance == huart3.Instance)//物联网
//	{
//		if(RX[0]=='1')
//		{
//			LED_ON(5);
//			Flag_Down = 1;
//		}
//		else if(RX[0]=='2')
//		{
//			LED_ON(6);
//			Flag_Mid = 1;
//			LED_ON(2);
//		}
//		else if(RX[0]=='3')
//		{
//			LED_ON(7);
//			Flag_Up = 1;
//		}
//		else if(RX[0]=='4')
//		{
//			LED_ON(8);
//			Flag_MidUpMid = 1;
//		}	
//		else if(RX[0]=='0')
//		{
//			
//		}
//		HAL_UART_Receive_IT(&huart3, (uint8_t *)RX, 1);		
//	}
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
  MX_ADC2_Init();
  MX_TIM6_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
 
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_Base_Start_IT(&htim6);//打开定时器6中断，完成花瓣合拢等过程
  //定时器3用于输出占空比控制灯
  //定时器2超声波计时
  
  HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_1);//定时器17用于输入捕获获取超声波数据
  
  HAL_UART_Receive_IT(&huart3, (uint8_t *)RX, 1);//串口3接收esp8266数据
  HAL_UART_Receive_IT(&huart2, (uint8_t *)RX, 1);//串口2，语音模块

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
	  
	  //超声波深度探测
	  gases_start_signal();
	  
	  if(Distance<SR04_Max){Percent_Rubbish = 100;}
	  else if(Distance>SR04_Max&&Distance<SR04_Min)Percent_Rubbish = (uint16_t)(((double)(SR04_Min*1.0 - Distance) / (SR04_Min-SR04_Max)) * 100.0);
	  else if(Distance>SR04_Min)Percent_Rubbish = 0;
	  //当垃圾满，则花瓣合拢
//	  if(Percent_Rubbish >= 99)
//	  {
//		  Flag_Up = 1;
//		  LED_ON(7);
//		  Flag_Percent = 1;
//	  }
//	  else 
//	  {
//		  Flag_Percent = 0;
//	  }
	  
	  sprintf((char *)Str_Esp8266, "%2d%2d%d", Temperature, Humidity, Percent_Rubbish);	  
	  HAL_UART_Transmit_IT(&huart3,(uint8_t *)Str_Esp8266,strlen((char *)Str_Esp8266));

	  //检测光照度，亮灯，防撞
	  Light = getADC_PA5();
	  LED_Show();
	  
	  //LCD显示信息
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	int i;
	UNUSED(htim);
 
	if(htim->Instance == htim17.Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(Channel2Edge == 0)          
			{
				Channel2RisingTime = HAL_TIM_ReadCapturedValue(&htim17, TIM_CHANNEL_1);                         
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim17, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);        
				HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_2);     
 
				Channel2Edge = 1;          	 
			}
			else if(Channel2Edge == 1)     
			{
				Channel2FallingTime = HAL_TIM_ReadCapturedValue(&htim17, TIM_CHANNEL_1);                       
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim17, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);        
				HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_1);   	 
 
				Channel2HighTime = Channel2FallingTime < Channel2RisingTime ? Channel2FallingTime + 0xffff - Channel2RisingTime + 1 : Channel2FallingTime - Channel2RisingTime;
				Distance = Channel2HighTime * 17 / 100; 
				Channel2Edge = 0;		  			
			}
		}
	}
//	array[i] = Channel2HighTime * 17 / 100;
	
//	if(i == 49)
//	{
//		for(int j = 0; j < 50; j++)
//		{
//			Distance += array[i];
//		}
//		Distance  = Distance * 1.0 / 50; 
//	}
//	
//	i++;
//	if(i==50)i=0;
}

void gases_start_signal(void)
{  
    TRIG_ON;
    //延时20us
    HAL_Delay(1);
    TRIG_OFF;
}

void LED_Show(void)
{
	if(Light > 1.60 && Light <= 1.80)
	  {
		  	RGB_Show_ALL(table[6]);
	  }
	  else if(Light > 1.80 && Light <= 2.00)
	  {
		  	RGB_Show_ALL(table[7]);
	  }
	  else if(Light > 2.00 && Light <= 2.20)
	  {
		  	RGB_Show_ALL(table[8]);
	  }
	  else if(Light > 2.20 && Light <= 2.40)
	  {
		  	RGB_Show_ALL(table[13]);
	  }
	  else if(Light > 2.40 && Light <= 2.60)
	  {
		  	RGB_Show_ALL(table[12]);
	  }
	  else if(Light > 2.60 && Light <= 2.80)
	  {
		  	RGB_Show_ALL(table[11]);
	  }
	  else if(Light > 2.80 && Light <= 3.0)
	  {
		  	RGB_Show_ALL(table[9]);
	  }
	  else if(Light > 3.00 && Light <= 3.20)
	  {
		  	RGB_Show_ALL(table[10]);
	  }
	  else if(Light > 3.20)RGB_Show_ALL(table[14]);	
	  else RGB_Show_ALL(BLACK);
}

void LCD_Show(void)
{
//	sprintf((char *)Str,  "Te_Hu_Pe:%2d,%2d,%d", Temperature, Humidity, Percent_Rubbish);
//    LCD_DisplayStringLine(Line0, Str);  
//	if(RX[0] == '0' || RX[0] == '1' || RX[0] == '2' || RX[0] == '3' || RX[0] == '4')
//		LCD_DisplayStringLine(Line1, (uint8_t *)(RX));
//	else LCD_DisplayStringLine(Line1, (uint8_t *)"0");
//	sprintf((char *)Str, "Dis:%2dcm,Light:%.2f", Distance, Light);
//    LCD_DisplayStringLine(Line2, (uint8_t *)Str);
//	sprintf((char *)Str, "K210_Have:%4d", Have_Count);
//    LCD_DisplayStringLine(Line3, (uint8_t *)Str);
//	
//	sprintf((char *)Str, "Check-Run:%d,%d", Flag_Check, Flag_Run);
//    LCD_DisplayStringLine(Line4, (uint8_t *)Str);
//	sprintf((char *)Str, "UpMdDnAc:%d%d%d%d", Flag_Up, Flag_Mid, Flag_Down, Flag_MidUpMid);
//    LCD_DisplayStringLine(Line5, (uint8_t *)Str);
//	
////	sprintf((char *)Str, "Angle:%3d", Angle);
////    LCD_DisplayStringLine(Line6, (uint8_t *)Str);
//	
//    sprintf((char *)Str, "Ema_C:%4d", Ema_Count);
//    LCD_DisplayStringLine(Line7, (uint8_t *)Str);
//	sprintf((char *)Str, "Stop_Time:%4d", Stop_Time_Count);
//    LCD_DisplayStringLine(Line8, (uint8_t *)Str);
//    sprintf((char *)Str, "DIR:%d", HAL_GPIO_ReadPin(DIR_Port, DIR_Pin));
//    LCD_DisplayStringLine(Line9, (uint8_t *)Str); 

    sprintf((char *)Str, "%d,%d,%d,%d%%,%dcm", K210, HAL_GPIO_ReadPin(DIR_Port, DIR_Pin),Have_Count, Percent_Rubbish,Distance);
	LCD_DisplayStringLine(Line6, (uint8_t *)Str);
	
//	sprintf((char *)Str, "%d,%d", K210,Have_Count);
//	LCD_DisplayStringLine(Line6, (uint8_t *)Str);
}

double getADC_PA5(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return adc * 3.3 / 4096;
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
