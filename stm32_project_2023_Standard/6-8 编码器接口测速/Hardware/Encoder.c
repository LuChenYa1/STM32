#include "stm32f10x.h"                  // Device header

/**
  * @brief  初始化，编码器旋转，控制定时器CNT自增自减
  * @param  无
  * @retval 无
  */
void Encoder_Init(void)
{
	//1、使能定时器，但还没有配置完，定时器不会工作
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //使能Tim3，TIM2用于输出PWM波形，不能再用于输入捕获
	 

	//2、配置GPIO PA6、PA7：TIM3_CH1 、TIM3_CH2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	               //上拉输入模式         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//3、配置定时器的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频，和时基单元关系不大（不需要，所以写1吧）
	//外部时钟源需要滤波，滤波需要采样频率，采样频率由内部时钟和时钟分频提供，这里是内部时钟源，不需要时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数,由编码器托管，手动设置无用
	TIM_TimeBaseInitStructure.TIM_Period = 65535 - 1;              //周期，自动重装值ARR,一般调最大用于CNT计数
	//这里有个小细节，写65536-1，会导致计数无法显示负数，出现+65532之类的
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;               //编码器用不着，设0
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;           //重复计数器，高级定时器的功能，这里没有，直接给零
	 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //到这一步，已经能正常计数 
	
	
	//4、配置输入捕获单元和编码器相关的参数
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);         //全部赋值，方便部分修改
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//TIM3_CH1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;         //滤波选择最大，尽量去毛刺
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//TIM3_CH1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;         //滤波选择最大，尽量去毛刺
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	//5、编码器配置
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//不反相
	//这里Rising变为Falling，可以改变旋转计数方向 
	
	//6、开启定时器
	TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  实时读取CNT值并清零 
  * @param  无
  * @retval CNT：计数值
  */
int16_t Encoder_Get(void)
{
	int16_t temp;
	temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3, 0);
	return temp;
}
