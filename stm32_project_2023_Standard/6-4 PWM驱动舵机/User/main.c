#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "PWM.h"
#include "Servo.h"

uint8_t KeyNum, i = 0;

int main(void)
{
	OLED_Init();
	Key_Init();
	Servo_init();
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			i ++;
			if(i == 6)i = 1;
		}
		if(i == 1)Servo_left90();
		if(i == 2)Servo_left45();
		if(i == 3)Servo_middle0();
		if(i == 4)Servo_right45();
		if(i == 5)Servo_right90();
		if(i)OLED_ShowNum(2, 1, (i-1)*45, 4);
	}
}
