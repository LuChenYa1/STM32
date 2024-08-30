#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint16_t CountSensor_count ;

void CountSensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//����B14Ϊ��������
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//����B14Ϊ�ⲿ�ж����ţ������жϴ��������ʹ�������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//AFIO
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      //�����ж϶����¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_Init(&EXTI_InitStructure);
	
	//����ͨ��14�����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //���ȼ�����2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;     //pin14��stm32f10x_exit.h_459
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t Get_SensorCounter(void)
{
	return CountSensor_count;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)//�ж��Ƿ�����ͨ��14������ж�
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)   //����ʱDO����������������ߵ�ƽ
		{
			CountSensor_count++;	
			EXTI_ClearITPendingBit(EXTI_Line14);             //�ֶ���λ
		}
		
	}
	
}
