#include "stm32f10x.h"                  // Device header


/**
  * @brief  1s定时器初始化函数
  * @param  无
  * @retval 无
  */
void Timer_Init(void)
{
	//1、选择内部时钟源（一般是默认内部时钟源）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);           //使能Tim2
	TIM_InternalClockConfig(TIM2);                                 
	
	//2、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频，和时基单元关系不大（不需要，所以随便写）
	//外部时钟源需要滤波，滤波需要采样频率，采样频率由内部时钟和时钟分频提供，这里是内部时钟源，不需要时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 7200 - 1;               //周期，自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能，这里没有，直接给零
	 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);            //到这一步，已经能正常计数计时，接下来是计数溢出触发中断
	//由于预分频器的缓冲寄存器的作用，只有更新事件/中断时预分频器的值才会起作用，
	//为使计时时间一致，一旦上电，手动生成更新事件使值有效，
	//但坏处是一上电就触发中断执行程序，刚初始化完就进中断，导致计次少一
	//解决方法是手动在配置中断前清除标志位，
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	//3、使能计数器的中断输出/更新中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//第二个参数是中断源选择，更新中断针对定时器溢出后重装，其他针对外设，例如电平变化等
	
	//4、NVIC配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStructure);
	
	//5、启动配置好了时基单元和中断触发的定时器
	TIM_Cmd(TIM2, ENABLE);
}

//中断函数模板
//void TIM2_IRQHandler()
//{
//	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//}

