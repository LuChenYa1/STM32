#include "stm32f10x.h"                  // Device header

/**
  * @brief  对定时器2的输出比较模式通道3初始化：单位时间间隔（）s
  * @param  无
  * @retval 无
  */
void PWM_Init(void)
{
	//1、选择内部时钟源（一般是默认内部时钟源）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);           //使能Tim2
	

	
	//将PA2配置为复用推挽输出，其控制权由输出数据寄存器转移给输出比较模式下的定时器，由手动变自动
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	               //复用推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2); 
	
	
	//2、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频，和时基单元关系不大（不需要，所以随便写）
	//外部时钟源需要滤波，滤波需要采样频率，采样频率由内部时钟和时钟分频提供，这里是内部时钟源，不需要时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                //周期，自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能，这里没有，直接给零
	 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);            //到这一步，已经能正常计数计时，接下来是计数溢出触发中断
	 
	//3、配置输出比较模式
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	//因为后面只需要对部分成员进行赋值，但结构体成员全部都需要赋值，
	//所以干脆用初始化函数一次性全部赋值，后面再对需要的部分成员赋值
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              //count < CCR,置 1（有效电平）；count > CCR,置 0（无效电平）
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      //极性不取反，有效电平为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //使能
	TIM_OCInitStructure.TIM_Pulse = 0;                             //CCR值/（ARR + 1），即占空比
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);                       //这里已经默认使用输出比较通道1，对应TIM2_CH1,复用引脚PA0
	 
	//OC_CH4 -> TIM2_CH4 -> PA3  输出比较模式是内嵌在定时器里的一些寄存器之类的
	
	//5、启动配置好了时基单元和输出比较模式的定时器
	TIM_Cmd(TIM2, ENABLE)  ;
}

/**
  * @brief  设置定时器2通道3的占空比（不取反）
  * @param  Compare:占空比
  * @retval 无
  */
void PWM_SetCompare3(uint8_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);
}
