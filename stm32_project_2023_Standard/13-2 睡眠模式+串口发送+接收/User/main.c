#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint8_t RXData = 0x00;

//魔术棒 >>> Debug >>> ST_Link Debugger >>> Settings >>> SW Device 
//在睡眠模式下，框内显示No Target Connected

int main(void)
{
	OLED_Init();
	Serial_Init();
	OLED_ShowString(1, 1, "RxData:");
 
	while (1)
	{
		if(Serial_GetRxFlag() == 1)
		{
			RXData = Serial_GetRxData();
			Serial_SendByte(RXData);//回传
			OLED_ShowHexNum(1, 8, RXData, 2);
		}
		
		//每进一次while循环，闪烁一次Runing
		OLED_ShowString(2, 1, "Runing");
		Delay_ms(500);
		OLED_ShowString(2, 1, "      ");
		Delay_ms(500);
		
		//进入由任意中断唤醒的睡眠模式
		if(RXData == 0x00)
		{
			//0x00是安全位，发送0x00可解除睡眠模式
			//当RxData是非0x66的数时，程序进入else执行睡眠，睡眠模式下新程序无法下载
			//解决方法：
			//1、在唤醒状态时赶紧下载程序以解除睡眠
			//2、利用if else结构设置安全位，在需要解除睡眠模式时，发送安全位唤醒设备，同时跳过睡眠操作
		}
		else 
		{
			Serial_SendByte(0x01);//在电脑端接收到01，说明此时设备进入睡眠模式
			__WFI();
		}
	}
}
