#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Serial.h"                  // Device header

void ChaoShengBo_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//trig输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//echo输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//定时器从echo返回高电平时开始计时
	TIM_InternalClockConfig(TIM1);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1;              //周期，自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能 
	 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);     
}

uint16_t Track_Distance(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	Delay_us(20);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET);
	TIM1->CNT = 0;
	TIM_Cmd(TIM1, ENABLE);//开始计时
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET);
	TIM_Cmd(TIM1, DISABLE);
	uint16_t CNT = TIM_GetCounter(TIM1);
	TIM_SetCounter(TIM1, 0);
	Delay_ms(10);
	return CNT;
}
