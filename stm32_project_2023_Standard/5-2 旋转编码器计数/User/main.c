#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "Encoder.h"


uint16_t num;

int main(void)
{
	OLED_Init();
	Encoder_Init();
	
	OLED_ShowString(1, 1, "num:");
	while(1)
	{
		 num += EncoderCount_Get();
		 OLED_ShowSignedNum(1, 5, num, 5);
	}
}
