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
 

void MySPI_Init(void)
{
	//1、时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	//2、GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//软件IO
	GPIO_InitStructure.GPIO_Pin = MySPI_SS_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MySPI_CLK_Port, &GPIO_InitStructure);
	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //硬件复用
	GPIO_InitStructure.GPIO_Pin = MySPI_MOSI_Pin | MySPI_CLK_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MySPI_CLK_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //硬件IO
	GPIO_InitStructure.GPIO_Pin = MySPI_MISO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MySPI_CLK_Port, &GPIO_InitStructure);
	
	//3、SPI结构体配置
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       //主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //八位数据帧
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  //高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//APB2外设时钟72MHZ的128分频
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        //CPHA = 0
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          //CPOL = 0
	SPI_InitStructure.SPI_CRCPolynomial = 7;                            //不用，给默认值7
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //硬件复杂，故采用软件模拟
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//4、总开关
	SPI_Cmd(SPI1, ENABLE);
	
	//5、SS默认高电平
	MySPI_W_SS(1);
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
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);//等待TDR空，可以发送数据
	SPI_I2S_SendData(SPI1, ByteSend);//写入操作清除标志位
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);//等待RDR非空，可以读取数据
	return SPI_I2S_ReceiveData(SPI1);//读取操作清除标志位
}
