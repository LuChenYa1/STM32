#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"


int main(void)
{
	OLED_Init();
	MyRTC_Init();
	
	OLED_ShowString(1, 1, "CNT :");
	OLED_ShowString(2, 1, "ALR :");
	OLED_ShowString(3, 1, "ALRF:");

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//该行在RTC有，但为了减少模块耦合性，再写一遍
	uint32_t Alarm = RTC_GetCounter() + 10;//定10秒以后的闹钟
	RTC_SetAlarm(Alarm);
	OLED_ShowNum(2, 6, Alarm, 10);
	
	
	PWR_WakeUpPinCmd(ENABLE);//PA0_WakeUp引脚置1，唤醒待机模式
	
	while (1)
	{
		OLED_ShowNum(1, 6, RTC_GetCounter(), 10);
		OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);
		
		OLED_ShowString(4, 1, "Runing...");
		Delay_ms(300);
		OLED_ShowString(4, 1, "         ");
		Delay_ms(300);
		
		OLED_ShowString(4, 10, "StandBy");
		Delay_ms(500);
		
		OLED_Clear();//清屏，减少电量损耗
		PWR_EnterSTANDBYMode();
		//待机模式只针对STM32本身（“断电状态”）
		//其挂载的外设虽然失去了时钟，但还有电，能保存最后一刻的寄存器数据
		//这个特性会使外设消耗STM32的电量，故应当即使清屏OLED
	}
}
