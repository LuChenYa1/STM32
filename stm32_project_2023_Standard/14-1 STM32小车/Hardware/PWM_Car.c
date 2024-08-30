#include "stm32f10x.h"                  // Device header


//Tim2_CH3、Tim2_CH2初始化,对应电机总开关PWMA、PWMB
void PWM_Init(void)
{
	//1、选择内部时钟源
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);           //使能Tim2
	
	//将PA2、PA1配置为复用推挽输出
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	               //复用推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	TIM_InternalClockConfig(TIM2); 
	
	
	//2、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                //周期，自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 9 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);            
	 
	//3、配置输出比较模式
	TIM_OCInitTypeDef TIM_OCInitStructure;                         //一次性全部赋值
	TIM_OCStructInit(&TIM_OCInitStructure);
	
	//CH3_PWMA_Left
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              //count < CCR,置 1（有效电平）；count > CCR,置 0（无效电平）
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      //有效电平为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
	TIM_OCInitStructure.TIM_Pulse = 0;                             //CCR值/（ARR + 1），即占空比
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);                        
	 
	//CH2_PWMB_Right
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;            
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                           
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);                     

	
	//5、启动配置好了时基单元和输出比较模式的定时器
	TIM_Cmd(TIM2, ENABLE);
}

 
//设置小车左轮速度  
void PWM_SetCompare3(uint8_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);
}

//设置小车轮速度
void PWM_SetCompare2(uint8_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);
}

