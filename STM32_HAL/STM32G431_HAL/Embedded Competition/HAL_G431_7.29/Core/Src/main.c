/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include <string.h>
#include <stdio.h>
#include "Emm_V5.h"
#include "DHT11.h"
#include "ws2812b.h"
#include "UART_IO.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ALL_Angle_90   Position_Control(0, 1, 30, 5, 850, 1, 0)//����λ��90��
#define ALL_Angle_150  Position_Control(0, 1, 30, 5, 1378, 1, 0)//����λ��160��
//#define ALL_Angle_0    Return_Origin(0, 0, 0)
#define ALL_Angle_0    Position_Control(0, 1, 30, 5, 0, 1, 0)//����λ��0��

#define PC6_PA3_UpUpUp HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)
#define PC7_PA5_UpDown HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
#define PC8_PA6_Downnn HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

#define PC9_PA2_LED_ON  HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)
#define PA8_PC4_LED_OFF HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)
#define PA9_PA1_FUN_OFF HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9)
#define PA10_PA0_FUN_ON HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)

//�������һ��ִ�У���ֱ��ִ�����Ľ��,��Ҫ��ʱ500ms
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RX_Uart1[50] = {0}, RX_Uart2[10] = {0}, RX_Uart3[50] = {0};//��ͬ�������ڽ������ݵ��ַ�����
uint8_t TX_Uart1[50] = {0}, TX_Uart3[50];//����1�������Ϳ����������3��������˫��ͨ��
uint8_t RecCount;//�������ݵ��ֽ���
uint8_t Str[50] = {0};//OLED��ʾ

uint8_t Flag_CAM_Run = 0, Flag_LED_Run = 0, Flag_Start_Count, Flag_Start_UpDown, Flag_LED_Revert, Flag_0_90, Flag_Mode, Flag_Mode_Timer;

uint16_t UpDown_Keep_Time, LED_Start_Time, ESP_NotCAM_Time, Mode_1_Start_Time, Count_CAM_Rubbish;

uint8_t Temperature, Humidity, Percent_Rubbish, Flag_Smelly_Level = 3;//��������

uint8_t Angle = 0;//ת���Ƕ�
uint8_t Distance;//����
double Light, Stink, ADC_Stink;//����
uint8_t Temperature = 0, Humidity = 0;//�¶ȣ�ʪ��
uint8_t RX_array[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_Show(void);
double getADC_PC0(void);
double getADC_PC2(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart2.Instance)//�����Ϣ����
	{
		HAL_UART_Receive_IT(&huart1, RX_Uart1, 50);
	}
	
	if(huart->Instance == huart2.Instance)//�Ӿ�������Ϣ����
	{	
		HAL_UART_Receive_IT(&huart2, (uint8_t *)RX_Uart2, 8);	
	}
	
	if(huart->Instance == huart3.Instance)//���������п��ƻ���
	{
		if(Flag_Mode == 1)//�ֶ�ģʽ������esp���п���
		{
			if(RX_Uart3[0]=='1')//1,����
			{
				Flag_CAM_Run = 1;
				ALL_Angle_0;
				Angle = 0;
			}
			else if(RX_Uart3[0]=='2')//2��ƽ��
			{
				Flag_CAM_Run = 1;
				ALL_Angle_90;
				Angle = 90;
			}
			else if(RX_Uart3[0]=='3')//3����£
			{
				Flag_CAM_Run = 1;
				ALL_Angle_150;
				Angle = 150;
			}
			else if(RX_Uart3[0]=='4')//4�� �ռ�
			{
				Flag_CAM_Run = 1;
				ALL_Angle_150;//��£
				Flag_Start_UpDown = 1;	
				if(Angle == 0 || Angle == 150)Flag_0_90 = 0;
				else if(Angle == 90)Flag_0_90 = 90;	
				Angle = 150;		
			}
		}
		if(RX_Uart3[0]=='5')//�Զ�ģʽ��Ĭ��ֻ��CAM����
		{
			Flag_Mode = 0;
		}
		else if(RX_Uart3[0]=='6')//�ֶ�ģʽ��Ĭ������������������
		{
			Flag_Mode = 1;
		}
		else if(RX_Uart3[0]=='7')//��ʱ�ռ���������Ӱ���Զ�/�ֶ�ģʽ
		{
			Flag_Mode_Timer = 1;
		}
		else if(RX_Uart3[0]=='8')//��ʱ�ռ��ر�
		{
			Flag_Mode_Timer = 0;
		}
		
		HAL_UART_Receive_IT(&huart3, (uint8_t *)RX_Uart3, 1);		
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(Flag_CAM_Run == 1)
	{
		if(++ESP_NotCAM_Time == 6000)
		{
			ESP_NotCAM_Time = 0;
			Flag_CAM_Run = 0;//���������������һ���Ժ����¿���CAM���
		}
	}
	
	if(Flag_Start_UpDown == 1)
	{
		Mode_1_Start_Time = 0;
		if(++UpDown_Keep_Time == 3000)//�ӵ����ʼ�˶�����
		{
			UpDown_Keep_Time = 0;
			if(Flag_0_90 == 0){ALL_Angle_0;Angle = 0;}
			else if(Flag_0_90 == 90){ALL_Angle_90;Angle = 90;}
			Flag_0_90 = 45;
			Flag_Start_UpDown = 0;
		}
	}
	
	if(Flag_Smelly_Level == 3)
	{
		if(++ LED_Start_Time == 1000)
		{
			LED_Start_Time = 0;
			Flag_LED_Revert = !Flag_LED_Revert;
			Flag_LED_Run = 0;
		}
		if(Flag_LED_Revert == 1)RGB_Show_ALL(RED);
		else RGB_Show_ALL(BLACK);
	}
	
	if(Flag_Mode_Timer == 1)
	{	
		if(++Mode_1_Start_Time == 20000)
		{
			Mode_1_Start_Time = 0;
			ALL_Angle_150;
		    if(Angle == 0 || Angle == 150)Flag_0_90 = 0;
		    else if(Angle == 90)Flag_0_90 = 90;				
			Angle = 150;
			Flag_Start_UpDown = 1;
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
  HAL_Delay(100);//��ʱ100ms

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  DHT11_Init();
  
  OLED_Init();//21�����ݣ�8�У��м��6���м��1
  OLED_Clear();
  OLED_Display_On();

  HAL_UART_Receive_IT(&huart1, RX_Uart1, 50);//���,ֻ����
  HAL_UART_Receive_IT(&huart2, RX_Uart2, 8);//MaixCAM��ֻ����
  HAL_UART_Receive_IT(&huart3, RX_Uart3, 1);//�����������շ�
  
  HAL_TIM_Base_Start_IT(&htim6);
  __HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);

  Flag_CAM_Run = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {  
///////////////////����///////////////////////////////////////////////////////////////////////
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//	  HAL_Delay(1000);
//	  Position_Control(0, 1, 50, 2, 800, 1, 0);//����λ��90��
//	  HAL_Delay(2000);
//	  Position_Control(0, 1, 40, 5, 0, 1, 0);//����λ��0��
	  
	  sprintf((char *)Str, (char *)"%3d %2d %2d %2d  ", RX_Uart2[0], RX_Uart2[2], RX_Uart2[4], RX_Uart2[6]);
	  OLED_ShowString(1, 6, (char *)Str, 12, 0);
	  sprintf((char *)Str, (char *)"%3d %2d %2d %2d C:%d ", RX_array[0], RX_array[1], RX_array[2], RX_array[3], Count_CAM_Rubbish);
	  OLED_ShowString(1, 7, (char *)Str, 12, 0);
	  
//	  HAL_UART_Transmit_IT(&huart2,(uint8_t *)"Hello World\r\n",strlen((char *)"Hello World\r\n"));
	  
/////////////////Maix CAM ��Ϣ����////////////////////////////////////////////////////////////
	  if(Flag_Mode == 0)//Ĭ���Զ�ģʽ
	  {
		  if(Flag_CAM_Run == 0)
		  {
			  for(int i = 0, j = 0; i < 8; i += 2)//����
			  {
				  if(RX_Uart2[i] == 0xff)RX_array[0] = 0xff;
				  if(RX_Uart2[i] == 0x55)RX_array[1] = 0x55;
				  
				  if(RX_Uart2[i] != 0xff && RX_Uart2[i] != 0x55)
				  {
					  RX_array[2+j] = RX_Uart2[i];
					  j = !j;
				  }
			  }
			  if(RX_array[0] == 0xff && RX_array[1] == 0x55)//�ж�
			  {
				  if(RX_array[2]!= 0)Distance = RX_array[2];//�����ж�
				  Percent_Rubbish = (125 - Distance) / 18.0 * 100;
				  sprintf((char *)Str, (char *)"Dist:%2d, Perc:%3d", Distance, Percent_Rubbish);
				  OLED_ShowString(0, 2, (char *)Str, 12, 0);
				  
				  if(RX_array[3] == 1)Count_CAM_Rubbish++;//�����ж�
				  if(Count_CAM_Rubbish == 5)
				  {
					  Count_CAM_Rubbish = 0;
					  OLED_ShowString(0, 0, "Rubbish", 12, 0);
					  Flag_CAM_Run = 1;//CAM�ر�
					  ALL_Angle_150;//��£
					  Flag_Start_UpDown = 1;
					  if(Angle == 0 || Angle == 150)Flag_0_90 = 0;
					  else if(Angle == 90)Flag_0_90 = 90;	
					  Angle = 150;						  
				  }
				  else OLED_ShowString(0, 0, "NO     ", 12, 0);
				  
				  RX_array[0] = 0;RX_array[1] = 0;RX_array[3] = 0;
				  RX_Uart2[0] = 0;RX_Uart2[1] = 0;RX_Uart2[2] = 0; RX_Uart2[3] = 0;
			  }				  
		  }
	  }
	  
///////////////////����������������/////////////////////////////////////////////////////////////
	  if(Flag_Mode == 1)//�ֶ�ģʽ
	  {
		  //////////����-���////////
		  if(PC6_PA3_UpUpUp == GPIO_PIN_SET)//չ��
		  { 
			  ALL_Angle_150;Angle = 150; 
			  Flag_CAM_Run = 1;//�ر�CAM���
		  }
		  if(PC7_PA5_UpDown == GPIO_PIN_SET)//�ռ�����
		  {
			  ALL_Angle_150;//��£
			  Flag_Start_UpDown = 1;
			  if(Angle == 0 || Angle == 150)Flag_0_90 = 0;
			  else if(Angle == 90)Flag_0_90 = 90;	
			  Angle = 150;
			  Flag_CAM_Run = 1;//�ر�CAM���
		  }	  
		  if(PC8_PA6_Downnn == GPIO_PIN_SET)//����
		  { 
			  ALL_Angle_0;Angle = 0;
			  Flag_CAM_Run = 1;//�ر�CAM���
		  }
		  
		  //////////����-LED//////////
		  if(PC9_PA2_LED_ON == GPIO_PIN_SET)//���ˣ�����
		  {
			  Flag_LED_Run = 1;
			  RGB_Show_ALL(table[13]);
		  }	  
		  if(PA8_PC4_LED_OFF == GPIO_PIN_SET)//���ˣ����
		  {
			  Flag_LED_Run = 0;
			  RGB_Show_ALL(BLACK);
		  }	  
		  if(PA9_PA1_FUN_OFF == GPIO_PIN_SET)
		  {
			  Flag_LED_Run = 1;//�Զ����ƹ��ܹر�
			  RGB_Show_ALL(BLACK);
		  }
		  if(PA10_PA0_FUN_ON == GPIO_PIN_SET)
		  {
			  Flag_LED_Run = 0;////�Զ����ƹ��ܿ���
			  RGB_Show_ALL(BLACK);
		  } 
	  }
	  sprintf((char *)Str, (char *)"%d %d %d %d %d %d %d ", PC6_PA3_UpUpUp, PC7_PA5_UpDown, PC8_PA6_Downnn, PC9_PA2_LED_ON, PA8_PC4_LED_OFF, PA9_PA1_FUN_OFF, PA10_PA0_FUN_ON);
	  OLED_ShowString(45, 0, (char *)Str, 12, 0); 
	  
//////////////////////////�Ƕ���ʾ//////////////////////////////////////////////////////////////

	  sprintf((char *)Str, (char *)"Angle: %3d    Mode:%d ", Angle, Flag_Mode);
	  OLED_ShowString(0, 1, (char *)Str, 12, 0);
	  
////////////////////��ʪ����Ϣ����//////////////////////////////////////////////////////////////
	  DHT11_Read_Data(&Temperature, &Humidity);
	  sprintf((char *)Str, (char *)"Tem:%2d Hum:%2d   CAM:%d", Temperature, Humidity, Flag_CAM_Run);
	  OLED_ShowString(0, 5, (char *)Str, 12, 0);
	  
///////////////////��ζŨ����Ϣ����/////////////////////////////////////////////////////////////
	  ADC_Stink = getADC_PC2();
	  Stink = (ADC_Stink / 3.3) * 49.5 + 0.5;
	  if(Stink < 5)Flag_Smelly_Level = 0;
	  if(Stink >= 5 && Stink < 7)Flag_Smelly_Level = 1;
	  else if(Stink >= 7 && Stink < 10)Flag_Smelly_Level = 2;
	  else if(Stink >= 10)
	  {
		  Flag_Smelly_Level = 3;
		  Flag_LED_Run = 1;
	  }
	  sprintf((char *)Str, (char *)"Stink:%2.2fppm  F:%d  ", Stink, Flag_Smelly_Level);
	  OLED_ShowString(0, 4, (char *)Str, 12, 0);

///////////////////����ǿ����Ϣ����/////////////////////////////////////////////////////////////
	  Light = getADC_PC0();
	  if(Flag_LED_Run == 0)LED_Show();//Ĭ�Ͽ���
	  sprintf((char *)Str, (char *)"Light:%.2f", Light);
	  OLED_ShowString(0, 3, (char *)Str, 12, 0);
	  
////////////////////���������ϴ���ʪ�ȣ�����ռ��, ��ζ�澯�̶�////////////////////////////////////////////////
	  sprintf((char *)TX_Uart3, "%2d%2d%3d%1d", Temperature, Humidity, Percent_Rubbish, Flag_Smelly_Level);	  
	  HAL_UART_Transmit_IT(&huart3,(uint8_t *)TX_Uart3,strlen((char *)TX_Uart3));
	  if(RX_Uart3[0] != '0' && RX_Uart3[0] != '1' && RX_Uart3[0] != '2' && RX_Uart3[0] != '3'&& RX_Uart3[0] != '4' && RX_Uart3[0] != '5' && RX_Uart3[0] != '6' && RX_Uart3[0] != '7' && RX_Uart3[0] != '8')
		  OLED_ShowString(115, 7, (char *)"0", 12, 0);
	  else
		  OLED_ShowString(115, 7, (char *)RX_Uart3, 12, 0);
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

/* USER CODE BEGIN 4 */

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

double getADC_PC2(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	adc = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	return adc * 3.3 / 4096;
}

double getADC_PC0(void)
{
	uint16_t adc;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 10);
	adc = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return adc * 3.3 / 4096;
}

//����
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//	  HAL_Delay(500);
//	  OLED_ShowNum(28, 0, RX_Uart2[0], 3, 12, 0);
//	  OLED_ShowNum(48, 0, RX_Uart2[2], 3, 12, 0);
//	  OLED_ShowNum(68, 0, RX_Uart2[4], 3, 12, 0);
//	  OLED_ShowNum(88, 0, RX_Uart2[6], 3, 12, 0);
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
