#include "stm32f10x.h"                  // Device header


#define MySPI_SS_Pin     GPIO_Pin_4  //输出
#define MySPI_MISO_Pin   GPIO_Pin_6  //输入
#define MySPI_MOSI_Pin   GPIO_Pin_7  //输出
#define MySPI_CLK_Pin    GPIO_Pin_5  //输出

#define MySPI_SS_Port    GPIOA
#define MySPI_MISO_Port  GPIOA
#define MySPI_MOSI_Port  GPIOA
#define MySPI_CLK_Port   GPIOA


//向SS写入一位数据
void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(MySPI_SS_Port, MySPI_SS_Pin, (BitAction)BitValue);
	//传输速率快，不需要加延时
}
//向MOSI写入一位数据
void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(MySPI_MOSI_Port, MySPI_MOSI_Pin, (BitAction)BitValue);
}
//向CLK写入一位数据
void MySPI_W_CLK(uint8_t BitValue)
{
	GPIO_WriteBit(MySPI_CLK_Port, MySPI_CLK_Pin, (BitAction)BitValue);
}
//读取MISO的一位数据
uint8_t MySPI_R_MISO(void)
{
	return  GPIO_ReadInputDataBit(MySPI_MISO_Port, MySPI_MISO_Pin);
}


void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MySPI_SS_Pin | MySPI_CLK_Pin | MySPI_MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MySPI_CLK_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = MySPI_MISO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MySPI_CLK_Port, &GPIO_InitStructure);
	
	MySPI_W_SS(1);//默认与设备断开联系
	MySPI_W_CLK(0);//模式0: 极性不反转，初始为低电平
}


void MySPI_Start(void)
{
	MySPI_W_SS(0);//选中设备，建立通信桥梁
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1);//不选中设备，停止通信
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00, i;
	
	for(i = 0;i < 8; i++)
	{
		MySPI_W_MOSI(ByteSend & (0x80 >> i));//CLK下降沿，移出数据，初始时会自动移出一位数据 
		MySPI_W_CLK(1);
		if(MySPI_R_MISO() == 1)ByteReceive |= (0x80 >> i);//CLK上升沿，接收数据
		MySPI_W_CLK(0);
	}
	return ByteReceive;
}
