#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "Buzzer.h"
#include "LightSensor.h"                  // Device header

uint8_t KeyNum;

int main(void)
{
	Buzzer_Init();
	LightSensor_Init();
	while(1)
	{ 
		if(LightSensor_Get()==0)
		{
			Buzzer_ON();
			Delay_ms(500);
			Buzzer_OFF();
			Delay_ms(500);
		}
		else
		{
			Buzzer_ON();
			Delay_ms(100);
			Buzzer_OFF();
			Delay_ms(100);
			Buzzer_ON();
			Delay_ms(100);
			Buzzer_OFF();
			Delay_ms(700);
		}
	}
}
