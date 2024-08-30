#include "stm32f10x.h"                  // Device header
#include "Delay.h"                  // Device header

unsigned char i;
int main(void)
{
	//IO口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOA, GPIO_Pin_0);                 //置低电平
	//GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);       //置低电平 
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);                    //置高电平
	//GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);         //置高电平
	while(1)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);  
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);      
		Delay_ms(100);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);  
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);      
		Delay_ms(700);
	}
}

