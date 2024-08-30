#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"                  // Device header


//ITConfig:使能外设的中断输出
int16_t Speed;

int main(void)
{
	OLED_Init();
	Timer_Init();
	Encoder_Init();
	//OLED_ShowString(1, 1, "CNT: ");
	OLED_ShowString(2, 1, "Speed: ");
	while(1)
	{
		//OLED_ShowSignedNum(1, 6, TIM_GetCounter(TIM3), 5);//读取计数值，最大是自动重装值65535 - 1
		//中断每隔1s对CNT清零，这里显示CNT没啥意义
		OLED_ShowSignedNum(2, 8, Speed, 3);
	}
}

//中断函数模板
void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Speed = Encoder_Get();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
