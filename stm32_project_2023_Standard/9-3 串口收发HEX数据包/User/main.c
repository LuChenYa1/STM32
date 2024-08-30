#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
 
//想打印中文时，串口助手和keil都要选择UTF_8,同时魔术棒——C/C++——路径选择下方——填入--no-multibyte-chars
//方法二：串口助手和keil选择中文编码GB2312（GBK兼容），要打印汉字的文件关闭再打开（刷新），写入文字即可>>>可行性不高，放弃

uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	Serial_Init();
    Key_Init();
	
	OLED_ShowString(1, 1, "RxPacket");
	OLED_ShowString(3, 1, "TxPacket");
	
	while (1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum)//一旦按下，发送数据包的值++，发送并用OLED显示
		{
			Serial_TxPacket[0] ++;
			Serial_TxPacket[1] ++;
			Serial_TxPacket[2] ++;
			Serial_TxPacket[3] ++;
			
			Serial_SendPacket(); 
			
			OLED_ShowHexNum(2, 1, Serial_TxPacket[0], 2);
			OLED_ShowHexNum(2, 4, Serial_TxPacket[1], 2);
			OLED_ShowHexNum(2, 7, Serial_TxPacket[2], 2);
			OLED_ShowHexNum(2, 10, Serial_TxPacket[3], 2);
		}
		if(Serial_GetRxFlag() == 1)
		{
			OLED_ShowHexNum(4, 1, Serial_RxPacket[0], 2);
			OLED_ShowHexNum(4, 4, Serial_RxPacket[1], 2);
			OLED_ShowHexNum(4, 7, Serial_RxPacket[2], 2);
			OLED_ShowHexNum(4, 10, Serial_RxPacket[3], 2);
		}
		 
	}
}
