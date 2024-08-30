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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "lcd.h"
#include <stdio.h>
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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
int32_t Freq_Value_2, Freq_Value_2_Now, Freq_2_Past, Freq_2_Now, Freq_Value_3, Freq_Value_3_Now, Freq_3_Past,Freq_3_Now, PX, Freq_Max_2, Freq_Min_2, Freq_Max_3, Freq_Min_3;
uint8_t Str[20];
Key_State KeyState = Key_Check;
uint8_t KeyNum, NDA, NDB, NHA, NHB;
uint16_t Key_Press_Time = 20, PH = 5000, PD = 1000;
uint8_t ShowSelect, Show_PF, Flag_PDHX, Flag_PH_2, Flag_PH_3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void Key_Scan(void);
void LCD_Show(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance == TIM2)
	{
		Freq_Value_2 = __HAL_TIM_GetCounter(&htim2);
		__HAL_TIM_SetCounter(&htim2, 0);
		Freq_2_Past = Freq_2_Now;
		Freq_2_Now = (1000000 / Freq_Value_2) + PX;
		Freq_Value_2_Now = (int32_t)(1000000 / Freq_2_Now);
		HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	}
	else if(htim ->Instance == TIM3)
	{
		Freq_Value_3 = __HAL_TIM_GetCounter(&htim3);
		__HAL_TIM_SetCounter(&htim3, 0);
		Freq_3_Past = Freq_3_Now;
		Freq_3_Now = (1000000 / Freq_Value_3) + PX;
		Freq_Value_3_Now = (int32_t)(1000000 / Freq_3_Now);
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  LED_Init();
  
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
   
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
			  if(ShowSelect == 1)
			  {
				  if(Flag_PDHX == 2)
				  {
					  PX += 100;
					  if(PX > 1000)PX = -1000;
				  }
				  else if(Flag_PDHX == 1)
				  {
					  PH += 100;
					  if(PH > 10000)PH = 1000;
				  }
				  else if(Flag_PDHX == 0)
				  {
					  PD += 100;
					  if(PD > 1000)PD = 100;
				  }
			  }
		  }
		  else if(KeyNum == 2)
		  {
			  if(ShowSelect == 1)
			  {
				  if(Flag_PDHX == 2)
				 {
					 PX -= 100;
					 if(PX < -1000)PX = 1000;
				 }
				 else if(Flag_PDHX == 1)
				 {
					 PH -= 100;
					 if(PH < 1000)PH = 10000;
				 }
				 else if(Flag_PDHX == 0)
				 {
					 PD -= 100;
					 if(PD < 100)PD = 1000;
				 }
			  }
		  }
		  else if(KeyNum == 3 && Key_Press_Time <= 1000)
		  {
			  if(ShowSelect == 1)
			  {
				  Flag_PDHX ++;
				  Flag_PDHX %= 3;
			  }
			  else if(ShowSelect == 0)
			  {
				  Show_PF = !Show_PF;
			  }
		  }
		  else if(KeyNum == 3 && Key_Press_Time > 1000)
		  {
			  if(ShowSelect == 2)
			  {
				  NDA = 0;NDB = 0;NHA = 0;NHB = 0;
			  }
		  }
		  else if(KeyNum == 4)
		  {
			  ShowSelect ++;
			  ShowSelect %= 3;
			  if(ShowSelect == 0)Show_PF = 0;
			  else if(ShowSelect == 1)Flag_PDHX = 0;
		  }
		  Key_Press_Time = 0;
	  }
	  /*****************************************************/
	  if(Freq_2_Now >= PH)
	  {
		  if(Flag_PH_2 == 1)
		  {
			  NHA++;
			  Flag_PH_2 = 0;
		  }
	  }
	  else 
	  {
		  Flag_PH_2 = 1;
	  }

	  if(Freq_3_Now >= PH)
	  {
		  if(Flag_PH_3 == 1)
		  {
			  NHB ++;
			  Flag_PH_3 = 0;
		  }
	  }
	  else 
	  {
		  Flag_PH_3 = 1;
	  }
	  /*****************************************************/
	  if(ShowSelect == 0)LED_ON(1);
	  else LED_OFF(1);
	  if(Freq_2_Now > PH)LED_ON(2);
	  else LED_OFF(2);
	  if(Freq_3_Now > PH)LED_ON(3);
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
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 79;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 PC8
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void LCD_Show(void)
{
	if(ShowSelect == 0)//频率/周期显示
	  {
		  if(Show_PF == 0)
		  {
			  LCD_DisplayStringLine(Line1, (uint8_t *)"        DATA      ");
			  if(Freq_2_Now <= 1000)
			  {
				  if(Freq_2_Now < 0)
				  {
					  LCD_DisplayStringLine(Line3, (uint8_t *)"     A=NULL     ");
				  }
				  else
				  {
					  sprintf((char *)Str, "     A=%dHz   ", Freq_2_Now);
					  LCD_DisplayStringLine(Line3, Str);
				  } 
			  }
			  else 
			  {
				  sprintf((char *)Str, "     A=%.2fKHz   ", Freq_2_Now * 1.0f / 1000);
				  LCD_DisplayStringLine(Line3, Str);
			  }
			  
			  if(Freq_3_Now <= 1000)
			  {
				  if(Freq_3_Now < 0)
				  {
					  LCD_DisplayStringLine(Line4, (uint8_t *)"     B=NULL   ");
				  }
				  else
				  {
					  sprintf((char *)Str, "     B=%dHz   ", Freq_3_Now);
					  LCD_DisplayStringLine(Line4, Str);
				  }
			  }
			  else 
			  {
				  sprintf((char *)Str, "     B=%.2fKHz   ", Freq_3_Now * 1.0f / 1000);
				  LCD_DisplayStringLine(Line4, Str);
			  }
			  LCD_DisplayStringLine(Line5, (uint8_t *)"                  ");
			  LCD_DisplayStringLine(Line6, (uint8_t *)"                  ");
		  }
		  else//周期
		  {
			  LCD_DisplayStringLine(Line1, (uint8_t *)"        DATA      ");
			  if(Freq_Value_2_Now <= 1000)
			  {
				  if(Freq_2_Now < 0)
				  {
					  LCD_DisplayStringLine(Line3, (uint8_t *)"     A=NULL   ");
				  }
				  else
				  {
					  sprintf((char *)Str, "     A:%duS     ", Freq_Value_2_Now);
					  LCD_DisplayStringLine(Line3, Str);
				  }
			  }
			  else 
			  {
				  sprintf((char *)Str, "     A=%.2fmS   ", Freq_Value_2_Now * 1.0f / 1000);
				  LCD_DisplayStringLine(Line3, Str);
			  }
			  
			  if(Freq_Value_3_Now <= 1000)
			  {
				  if(Freq_3_Now < 0)
				  {
					  LCD_DisplayStringLine(Line4, (uint8_t *)"     B=NULL   ");
				  }
				  else
				  {
					  sprintf((char *)Str, "     B=%duS     ", Freq_Value_3_Now);
					  LCD_DisplayStringLine(Line4, Str);
				  }
			  }
			  else 
			  {
				  sprintf((char *)Str, "     B=%.2fmS     ", Freq_Value_3_Now * 1.0f / 1000);
				  LCD_DisplayStringLine(Line4, Str);
			  }
			  LCD_DisplayStringLine(Line5, (uint8_t *)"                  ");
			  LCD_DisplayStringLine(Line6, (uint8_t *)"                  ");
		  }
	  }
	 
	  else if(ShowSelect == 1)//参数显示
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"        PARA      ");
		  sprintf((char *)Str, "     PD=%dHz      ", PD);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "     PH=%dHz      ", PH);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "     PX=%dHz      ", PX);
		  LCD_DisplayStringLine(Line5, Str);
		  LCD_DisplayStringLine(Line6, (uint8_t *)"                  ");
	  }
	  else if(ShowSelect == 2)//统计显示
	  {
		  LCD_DisplayStringLine(Line1, (uint8_t *)"        RECD      ");
		  sprintf((char *)Str, "     NDA=%d         ", NDA);
		  LCD_DisplayStringLine(Line3, Str);
		  sprintf((char *)Str, "     NDB=%d         ", NDB);
		  LCD_DisplayStringLine(Line4, Str);
		  sprintf((char *)Str, "     NHA=%d         ", NHA);
		  LCD_DisplayStringLine(Line5, Str);
		  sprintf((char *)Str, "     NHB=%d         ", NHB);
		  LCD_DisplayStringLine(Line6, Str);
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
