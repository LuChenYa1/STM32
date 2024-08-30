#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"

uint8_t KeyNum;

int main(void)
{
	LED_Init();
	Key_Init();
	while(1)
	{
		LED1_ON();
		KeyNum = Key_GetNum();
		if(KeyNum)
		{
			if(KeyNum == 1)
			{
				LED1_Turn();
				LED2_OFF();
			}
			if(KeyNum == 11)
			{
				LED1_OFF();
			    LED2_Turn();
			}
		}
	}
}
