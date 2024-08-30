#include "stm32f10x.h"                  // Device header


//舵机使用：
//需要20ms的脉冲，故周期（进中断所需时间）为20ms，频率为50HZ，
//f = 72M /（PSC + 1）/（ARR + 1）
//为方便计算，这里定ARR = 20 000，PSC = 72
//CCR = 500，0.5ms  转-90
//CCR = 1000，1ms   转-45
//CCR = 1500，1.5ms 转+0
//CCR = 2000，2ms   转+45
//CCR = 2500，2.5ms 转+90


/**
  * @brief  对定时器3的输出比较模式通道3初始化：单位时间间隔1s
  * @param  无
  * @retval 无
  */
void PWMServo_Init(void)
{
	//1、选择内部时钟源（一般是默认内部时钟源）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           
	TIM_InternalClockConfig(TIM3); 


	//将PA1配置为复用推挽输出
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	               //复用推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//2、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;              //周期，自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能，这里没有，直接给零
	 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             
	 
	//3、配置输出比较模式
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	//因为后面只需要对部分成员进行赋值，但结构体成员全部都需要赋值，
	//所以干脆用初始化函数一次性全部赋值，后面再对需要的部分成员赋值
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              //count < CCR,置 1（有效电平）；count > CCR,置 0（无效电平）
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      //极性不取反，有效电平为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //使能
	TIM_OCInitStructure.TIM_Pulse = 0;                             //CCR值/（ARR + 1），即占空比
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);                        
	 
	//OC_CH3 -> TIM3_CH3 -> PB0  
	
	//5、启动配置好了时基单元和输出比较模式的定时器
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  设置定时器3通道3的占空比（不取反）
  * @param  Compare:占空比
  * @retval 无
  */
void PWMServo_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM3, Compare);
}
