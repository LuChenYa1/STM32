#include "stm32f10x.h"                  // Device header


void USART3_Init(void)
{
	//USART3_TX PB10
	//USART3_RX PB11
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��GPIOB��USART3��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//��ʼ��B10���š���RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��ʼ��B11���š���TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);  

	//�����ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;                  					//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   					//�ֳ���λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;        					//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;           					//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                //�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure);   

	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //�������ڽ����ж�
	USART_Cmd(USART3, ENABLE);                     //ʹ�ܴ���3
	USART_ClearFlag(USART3, USART_FLAG_TC);        //�����־λ	
}


