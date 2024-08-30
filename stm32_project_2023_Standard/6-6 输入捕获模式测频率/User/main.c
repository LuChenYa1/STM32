#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"
 
uint16_t Freq;

int main(void)
{
	OLED_Init();
	PWM_Init(); 
	IC_Init();
	
	PWM_SetCompare1(50);//设置占空比 50 %
	PWM_SetPrescaler(720 - 1);//设置频率 1000 HZ
	
	
	OLED_ShowString(1, 1, "Freq:00000HZ");
	
	while(1)
	{
		Freq = IC_GetFreq();
		OLED_ShowNum(1, 6, Freq, 5);
	}
}
