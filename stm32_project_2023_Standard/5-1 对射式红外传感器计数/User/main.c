#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "CountSensor.h"

//挡光时，传感器向Pin14输入高电平，松开后变为低电平并触发下降沿中断，中断标志位置一，执行中断程序，标志位清零

int main(void)
{
	OLED_Init();
	CountSensor_Init();
	
	OLED_ShowString(1,1,"Count:");
	
	while(1)
	{
		OLED_ShowNum(1,7,Get_SensorCounter(),4);
	}
}

