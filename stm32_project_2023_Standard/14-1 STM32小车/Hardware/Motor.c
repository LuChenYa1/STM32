#include "stm32f10x.h"                  // Device header
#include "PWM_Car.h"

//继承PWM，中间层应用
//驱动电机


//右轮：总使能PA2，分开关PA4_AIN1、PA5_AIN2，输出比较通道3
//左轮：总使能PA1，分开关PA6_BIN1、PA7_BIN2，输出比较通道2
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init();
}


//设置右轮速度（ -100 ~ +100 ）
void Motor_SetRightSpeed(int8_t Speed)
{
	if(Speed >= 0)//正转
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5); 
		PWM_SetCompare3(Speed);
	}
	else//反转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		PWM_SetCompare3(-Speed); 		
	}
}

//设置左轮速度（ -100 ~ +100 ）
void Motor_SetLeftSpeed(int16_t Speed)
{
	if(Speed >= 0)//正转
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7); 
		PWM_SetCompare2(Speed);
	}
	else//反转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		PWM_SetCompare2(-Speed); 		
	}
}

