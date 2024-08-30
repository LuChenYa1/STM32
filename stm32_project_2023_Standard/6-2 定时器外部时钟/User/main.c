#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "Timer.h"

uint8_t Num;

//ITConfig:使能外设的中断输出

int main(void)
{
	OLED_Init();
	Timer_Init();
	
	OLED_ShowString(1, 1, "Num: ");
	OLED_ShowString(3, 1, "Count: ");

	while(1)
	{
		OLED_ShowNum(1, 6, Num, 3);//进入中断次数
		OLED_ShowNum(4, 1, Timer_GetCounter(), 5);//读取定时器计数值，最大是自动重装值6 - 1
	}
}

//中断函数模板
void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Num ++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
