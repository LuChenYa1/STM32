#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"
 
 

int main(void)
{
	OLED_Init();
	PWM_Init(); 
	IC_Init();
	
	PWM_SetPrescaler(720 - 1);//设置频率 1000 HZ
	PWM_SetCompare1(80);//设置占空比 80 %
	
	OLED_ShowString(1, 1, "Freq:00000HZ");
	OLED_ShowString(2, 1, "Duty:00%");
	
	while(1)
	{
		OLED_ShowNum(1, 6, IC_GetFreq(), 5);
		OLED_ShowNum(2, 6, IC_GetDuty(), 2);
	}
}
