#include "stm32f10x.h"                  // Device header


void IC_Init(void)
{
	//1、使能定时器，选择内部时钟源（一般是默认内部时钟源），但还没有配置完，定时器不会工作
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //使能Tim3，TIM2用于输出PWM波形，不能再用于输入捕获
	TIM_InternalClockConfig(TIM3); 


	//2、配置GPIO PA6：TIM3_CH1 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	               //上拉输入模式         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//3、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频，和时基单元关系不大（不需要，所以随便写）
	//外部时钟源需要滤波，滤波需要采样频率，采样频率由内部时钟和时钟分频提供，这里是内部时钟源，不需要时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 65535 - 1;                //周期，自动重装值ARR,调到最大，防止测较低频率时计数溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;        
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能，这里没有，直接给零
	 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //到这一步，已经能正常计数计时，接下来是计数溢出触发中断
	
	
	//4、配置输入捕获单元
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//TIM3_CH1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;//滤波选择最大，尽量去毛刺
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿触发
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//一分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//直连通道，CH1——>TI1FP1
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
	//PWMI模式只适用于TI1FI1和TI1FP2，这个函数会对另一个没配置的通道配置相反的参数，滤波和分频不变
	
	//5、配置TRGI的触发源为TI1FP1
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
	
	//打开总开关，开始工作
	TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  读取当前输入信号波形的频率
  * @param  无
  * @retval 1000000/TIM_GetCapture1(TIM3)：频率
  */
uint16_t IC_GetFreq(void)
{
	return 1000000/(TIM_GetCapture1(TIM3) + 1);
}


/**
  * @brief  读取当前输入信号波形的占空比
  * @param  无
  * @retval CCR2 / CCR1
  */
uint32_t IC_GetDuty(void)
{
	return (TIM_GetCapture2(TIM3) + 1) * 100 / (TIM_GetCapture1(TIM3) + 1);
}
