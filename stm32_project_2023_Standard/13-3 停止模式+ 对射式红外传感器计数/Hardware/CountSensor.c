#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint16_t CountSensor_count ;

void CountSensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//设置B14为上拉输入
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//设置B14为外部中断引脚，配置中断触发条件和触发内容
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//AFIO
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      //触发中断而非事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//设置通道14的优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //优先级分组2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;     //pin14，stm32f10x_exit.h_459
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t Get_SensorCounter(void)
{
	return CountSensor_count;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)//判断是否是由通道14进入的中断
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)   //挡光时DO数据线向主控输入高电平
		{
			CountSensor_count++;	
			EXTI_ClearITPendingBit(EXTI_Line14);             //手动复位
		}
		
	}
	
}
