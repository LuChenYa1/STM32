#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"

uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	//OLED_ShowNum(2,1,12345,5);
	
	while(1)
	{
		if(Key_GetNum1() == 1 && Key_GetNum2() == 1)
		{
			OLED_ShowString(1,1,"difang");
		}			
		if(Key_GetNum1() == 0 && Key_GetNum2() == 1)
		{
			OLED_ShowString(2,1,"True");
		} 
		if(Key_GetNum1() == 1 && Key_GetNum2() == 0)
		{
			OLED_ShowString(3,1,"False");
		}
	}
}
