#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "Timer.h"

uint8_t Num;

//ITConfig:使能外设的中断输出

int main(void)
{
	OLED_Init();
	Timer_Init();
	while(1)
	{
		OLED_ShowNum(1, 1, Num, 3);
		OLED_ShowNum(2, 1, TIM_GetCounter(TIM2), 5);//读取计数值，最大是自动重装值7200 - 1
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
