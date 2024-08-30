#include "tim.h"
#include "UART_IO.h"

#define USART_BAUDRATE 115200
#define USART_TIMING_HALF (uint16_t)((SystemCoreClock / (USART_BAUDRATE * 2)) - 1) // 半位时间
#define USART_TIMING_BIT (USART_TIMING_HALF * 2) // 一位时间

extern TIM_HandleTypeDef htim6; 

void USART_IO_SendByte(uint8_t data);
uint8_t USART_IO_ReceiveByte(void);
void USART_IO_SendString(const char *str);
void USART_IO_ReceiveString(char *str, uint16_t size);

// 一个字节
void USART_IO_SendByte(uint8_t data) 
{
    // 发送开始位
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);

    // 数据位
    for (int i = 0; i < 8; i++) 
	{
        if (data & 0x01) 
		{
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        }
		else 
		{
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        data >>= 1;
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);
    }

    // 停止位
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);
}

// 接收一位字节
uint8_t USART_IO_ReceiveByte(void) 
{
    uint8_t data = 0;
    // 等待开始位
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET);
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);

    //读取数据位
    for (int i = 0; i < 8; i++) 
	{
        data >>= 1;
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET)			
		{
            data |= 0x80;
        }
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);
    }

	//等待停止位
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < USART_TIMING_BIT);

    return data;
}

// 发送字符串
void USART_IO_SendString(const char *str) 
{
    while (*str) 
	{
        USART_IO_SendByte(*str++);
    }
}

// 接收字符串
void USART_IO_ReceiveString(char *str, uint16_t size) 
{
    uint16_t i = 0;
    while (i < (size - 1))
	{
        str[i] = USART_IO_ReceiveByte();
        if (str[i] == '\0' || str[i] == '\n') 
		{
            break;
        }
        i++;
    }
    str[i] = '\0'; 
}
