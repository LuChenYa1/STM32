#include "stm32f10x.h"                  // Device header


void USART3_Init(void)
{
	//USART3_TX PB10
	//USART3_RX PB11
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使能GPIOB、USART3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//初始化B10引脚——RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//初始化B11引脚——TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);  

	//串口中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                  					//配置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   					//字长八位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;        					//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;           					//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                //收发模式
	USART_Init(USART3, &USART_InitStructure);   

	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启串口接收中断
	USART_Cmd(USART3, ENABLE);                     //使能串口3
	USART_ClearFlag(USART3, USART_FLAG_TC);        //清除标志位	
}


