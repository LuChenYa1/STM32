#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

//注意一个小细节，接收到的数据是16位的，但低八位才是真实数据，所以可以用uint_8类型变量来接收数据

//小技巧，ctrl+AIT+（点按）空格
uint8_t Serial_TxPacket[4];//发送数据包载荷
uint8_t Serial_RxPacket[4];//接收数据包载荷
uint8_t Serial_RxFlag;  //检测是否接收完毕数据包

/**
  * @brief  串口1初始化，9600，8位，无校验
  * @param  无
  * @retval 无
  */
void Serial_Init(void)
{
	//1、开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 
	//2、配置GPIO,PA9->TX
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入模式，复用输入和普通输入都一样
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制，一般不用
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                //发送模式，两种都需要就用 | 括起来
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长，无校验，所以八位
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//选RXNE作为中断触发信号
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}


/**
  * @brief  发送一个字节
  * @param  Byte：字节
  * @retval 无
  */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);                               //该函数作用是发送一个字节（8位）数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待发送完成，防止数据覆盖
	//实验证明不加这一句，前面的数据会被后来的数据覆盖
}


/**
  * @brief  发送一个指定长度的数组
  * @param  *array:数组地址
  * @param  Length:数组长度
  * @retval 无
  */
void Serial_SendArray(uint8_t array[], uint16_t Length)
{
	uint16_t i;
	for(i = 0;i < Length; i++)
	{
		Serial_SendByte(array[i]);
	}
}


/**
  * @brief  发送字符串
  * @param  字符数组
  * @retval 无
  */
void Serial_SendString(uint8_t *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
	}
}


/**
  * @brief  X^Y
  * @param  X, Y
  * @retval 无
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while(Y--)
	{
		Result *= X; 
	}
	return Result;
}


/**
  * @brief  发送数字
  * @param  Number：数字
  * @param  Length:长度
  * @retval 无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length-1-i) % 10 + '0');
	}
}


/**
  * @brief  该函数是printf函数的底层，通过不断调用该函数完成打印功能，现将该函数重定向到串口1，实现printf输出到串口  
  * @param  ch:发送到串口的字符
  * @retval ch
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}


/**
  * @brief  封装sprintf
  * @param  字符串，（可变参数）
  * @retval 无
  */
void Serial_Printf(int8_t *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, (const char *)format, arg);
	va_end(arg);
	Serial_SendString((uint8_t*)String);
}


/**
  * @brief  判断是否进入接收中断，是则返回1，否则返回0；同时清除标志位
  * @param  无
  * @retval 1/0，是否进入中断
  */
uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}


/**
  * @brief  发送数据包
  * @param  无
  * @retval 无
  */
void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	Serial_SendByte(0xFE);
}


/**
  * @brief  USART1的接收数据中断，状态机思想
  * @param  无
  * @retval 无
  */
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		if(RxState == 0)//等待包头
		{
			if(RxData == 0xFF)
			{
				RxState = 1;
			}
			pRxPacket = 0;//将上一次数据包接收留下的pRxPacket清零（妙啊）
		}
		else if(RxState == 1)//接收指定长度的数据
		{
			Serial_RxPacket[pRxPacket++] = RxData;
			if(pRxPacket == 4)
			{
				RxState = 2;
			}
		}
		else if(RxState == 2)//等待包尾
		{
			if(RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1; //一个数据包接收完毕
			}
		}
		USART_ClearITPendingBit(USART1, USART_FLAG_RXNE);//清除哪个外设的哪个标志位,RXNE有读操作就清零，该句在该前提下可以不写
	}
}
