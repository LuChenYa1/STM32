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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	Key_Check,
	Key_Press,
	Key_Release,
	Key_Over,
}Key_State;//枚举一个类型，该类型的变量只能在括号中取值
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUZZ_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
#define BUZZ_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
#define LED1_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
#define LED1_OFF HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

#define B1       HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
#define B2		 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define B3       HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
#define B4 		 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t KeyNum;
Key_State KeyState = Key_Check;//给按键状态设置初始值
uint32_t Key_Press_Time = 20;//记录按压时间，单位ms
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Key(void)//短按检测
{
	uint8_t KeyNum = 0;
	if(B4 == GPIO_PIN_RESET)//按键4按下
	{	//松手检测模板
//		HAL_Delay(10);//消抖
//		while(B4 == GPIO_PIN_RESET);
//		KeyNum = 4;  

		HAL_Delay(10);//按下检测模板
		while(B4 == GPIO_PIN_RESET)KeyNum = 4;  
		while(!B4);//等待松手
	}
	if(B1 == GPIO_PIN_RESET)//按键1按下
	{
		HAL_Delay(10);
		while(B1 == GPIO_PIN_RESET);
		KeyNum = 1;  
	}
	if(B2 == GPIO_PIN_RESET)//按键2按下
	{
		HAL_Delay(10);
		while(B2 == GPIO_PIN_RESET);
		KeyNum = 2;  
	}
	if(B3 == GPIO_PIN_RESET)//按键3按下
	{
		HAL_Delay(10);
		while(B3 == GPIO_PIN_RESET);
		KeyNum = 3;  
	}
	return KeyNum;
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
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(KeyState == Key_Over)//只有松手后，即按压时间记录结束，该标志位才会置一，即为松手执行功能，带长按时间检测
	  {
		  KeyState = Key_Check;//保证以下程序只在刚按下时执行一次
		  if(KeyNum == 1 && Key_Press_Time > 1200){BUZZ_ON;}
		  else if(KeyNum == 1 && Key_Press_Time <1100){BUZZ_OFF;}
		  else if(KeyNum == 2){BUZZ_OFF;}
		  else if(KeyNum == 3)
		  {
			LED1_ON;
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);  
		  }
		  else if(KeyNum == 4){LED1_OFF;}
		  Key_Press_Time = 20;//按压时间“清零”，方便下次按压时计时
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
void Key_Scan(void)
{
	switch(KeyState)
	{
		case Key_Check://平常状态
		{
			if(B1 == GPIO_PIN_RESET || B2 == GPIO_PIN_RESET || B3 == GPIO_PIN_RESET || B4 == GPIO_PIN_RESET)
			{
				KeyState = Key_Press;//在检测状态下，检测是否有按键按下，如果有，转换为按下状态
			}
		};
		break;
		case Key_Press://在按下状态下，判断具体是哪个按键按下，给予键值
		{
			if(B1 == GPIO_PIN_RESET)KeyNum = 1;
			else if(B2 == GPIO_PIN_RESET)KeyNum = 2;
			else if(B3 == GPIO_PIN_RESET)KeyNum = 3;
			else if(B4 == GPIO_PIN_RESET)KeyNum = 4;
			KeyState = Key_Release;//不管松没松手，直接进入“松手”状态
		};
		break;
		case Key_Release://在“松手”状态下，判断是否真的松手
		{
			if(B1 == GPIO_PIN_RESET || B2 == GPIO_PIN_RESET || B3 == GPIO_PIN_RESET || B4 == GPIO_PIN_RESET)
			{
				Key_Press_Time += 10;//过了10ms，发现还没松手，按压时间+10
			}
			else//真的松手了，判断为短按
			{
				KeyState = Key_Over;//此时为按下后松手的状态
			}
		};
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
