#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "string.h"
#include "LED.h"

//想打印中文时，串口助手和keil都要选择UTF_8,同时魔术棒——C/C++——路径选择下方——填入--no-multibyte-chars
//方法二：串口助手和keil选择中文编码GB2312（GBK兼容），要打印汉字的文件关闭再打开（刷新），写入文字即可>>>可行性不高，放弃
 

int main(void)
{
	OLED_Init();
	Serial_Init();
    LED_Init();
	
	OLED_ShowString(1, 1, "RxPacket");
	 
	while (1)
	{ 
		if(Serial_RxFlag == 1)
		{
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, Serial_RxPacket);
			if(strcmp(Serial_RxPacket, "LED_ON") == 0)//接收到"LED_ON"
			{
				LED1_ON();
				Serial_SendString((uint8_t *)"LED_ON_OK\r\n");
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "LED_ON_OK");
			}
			else if(strcmp(Serial_RxPacket, "LED_OFF") == 0)//接收到"LED_OFF"
			{
				LED1_OFF();
				Serial_SendString((uint8_t *)"LED_OFF_OK\r\n");
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "LED_OFF_OK");
			}
			else  
			{
				Serial_SendString((uint8_t *)"ERROR_COMMAND\r\n");
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "ERROR_COMMAND");
			}
			Serial_RxFlag = 0; //读取完毕，标志位清零
		}
	}
}
