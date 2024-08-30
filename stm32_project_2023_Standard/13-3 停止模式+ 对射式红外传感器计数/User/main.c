#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "CountSensor.h"

  

int main(void)
{
	OLED_Init();
	CountSensor_Init();
	
	OLED_ShowString(1,1,"Count:");
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//开启电源外设
	
	
	
	while(1)
	{
		OLED_ShowNum(1,7,Get_SensorCounter(),4);
		
		OLED_ShowString(2, 1, "Runing...");
		Delay_ms(500);
		OLED_ShowString(2, 1, "         ");
		Delay_ms(500);
		
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//进入外部中断唤醒的停止模式
		//停止模式时钟全关了，只能使用不依赖时钟的外部中断唤醒
		SystemInit();//停止模式被唤醒后，系统时钟从HSE变为HSI，主频变为8MHZ,该函数能将主频变回72MHZ
	}
}

