#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint8_t RXData;
//想打印中文时，串口助手和keil都要选择UTF_8,同时魔术棒——C/C++——路径选择下方——填入--no-multibyte-chars
//方法二：串口助手和keil选择中文编码GB2312（GBK兼容），要打印汉字的文件关闭再打开（刷新），写入文字即可>>>可行性不高，放弃


int main(void)
{
	OLED_Init();
	Serial_Init();
	OLED_ShowString(1, 1, "RxData:");
 
	while (1)
	{
		//不断检测标志位RXNE以接收数据，适合简单数据接收
//		if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//		{
//			RXData = USART_ReceiveData(USART1);//两个数为一组进行接收
//			OLED_ShowHexNum(1, 1, RXData, 4);//实验证明，电脑端如果发送4143，则RXData的值一开始是41，延时后再接收数据，其值变为43
//			Delay_ms(500);
//			//对RXDR的读操作能清零标志位
//		}
		if(Serial_GetRxFlag() == 1)
		{
			RXData = Serial_GetRxData();
			Serial_SendByte(RXData);//回传
			OLED_ShowHexNum(1, 8, RXData, 2);
		}
		
	}
}
