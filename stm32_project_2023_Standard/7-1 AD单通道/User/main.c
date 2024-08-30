#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "AD.h"

uint16_t ADValue;
float Voltage;

int main(void)
{
	OLED_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "ADValue:");
	OLED_ShowString(2, 1, "Voltage:0.00V");
	while(1)
	{
		ADValue = ADC_GetValue();
		Voltage = (float)ADValue/4095*3.3;
		
		OLED_ShowNum(1, 9, ADValue, 4);
		//数据跳变是正常现象，但使用看门狗中断，利用阈值完成例如开关灯操作时，会出现灯闪的不良现象
		//解决方法：施密特滤波；均值滤波；左对齐减小分辨率（精度），把数据的尾数去掉
		//由于ADC内部硬件问题，ADValue最高时不能达到3.3V
		OLED_ShowNum(2, 9, Voltage, 1);                     //显示整数部分
		OLED_ShowNum(2, 11, (uint16_t)(Voltage*100)%100, 2);//显示小数部分
	}
}
