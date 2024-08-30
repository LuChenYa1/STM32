#include "stm32f10x.h"                  // Device header


uint16_t Encoder_Count;

void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//设置B1和B0为输入
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入(默认高电平)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//设置B0和B1为外部中断引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);//AFIO，复用IO口
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);//AFIO
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //使能
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;//配置外部中断通道
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //触发中断而非事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//配置通道1和0的优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //优先级分组2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;         //选择了外设pin0的中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;         //pin1	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;       //响应优先级
	NVIC_Init(&NVIC_InitStructure);
}


uint16_t EncoderCount_Get(void)
{
	uint16_t temp;
	temp = Encoder_Count;
	Encoder_Count = 0;
	return temp;
	//return Encoder_Count;
}


//startup_stm32f10x_md.s_183
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		//注意这里是读取IO口的输入值，不是输出值
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)//当引脚0触发下降沿，引脚1为低电平时，标记为正转
		{
			Encoder_Count --;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);//复位
	}
}


void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) == SET)
	{ 
		//注意这里是读取IO口的输入值，不是输出值		
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)//当引脚1触发下降沿，引脚0为低电平时，标记为正转
		{
			Encoder_Count ++;
		}
		EXTI_ClearITPendingBit(EXTI_Line1);//复位
	}
}

