#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"


//想打印中文时，串口助手和keil都要选择UTF_8,同时魔术棒——C/C++——路径选择下方——填入--no-multibyte-chars
//方法二：串口助手和keil选择中文编码GB2312（GBK兼容），要打印汉字的文件关闭再打开（刷新），写入文字即可>>>可行性不高，放弃


int main(void)
{
	OLED_Init();
	
	Serial_Init();
	
	Serial_SendByte(0x41);
	
	uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};
	Serial_SendArray(MyArray, 4);
	
	Serial_SendString((uint8_t*)"\r\nNum1=");//一定要\r\n配合使用才能换行
	
	Serial_SendNumber(111, 3);
	
//	printf("\r\nNum2=%d", 222);//实现常量字符和变量数字同时打印到串口，这个函数有大病！！！！！不要用
	
	//这三句可以封装为一个函数
	char String[100];//sprintf可以将字符串格式化输出到指定字符串
	sprintf(String, "\r\nNum2=%d", 222);
	Serial_SendString((uint8_t*)String);
	
	Serial_Printf((uint8_t*)"\r\nNum3=%d", 333);
	Serial_Printf((uint8_t*)"\r\n");
	while (1)
	{
		
	}
}
