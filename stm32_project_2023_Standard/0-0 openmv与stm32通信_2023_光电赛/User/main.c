#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "USART.h"

uint8_t length = 0, Flag = 0 ;

int main(void)
{
	OLED_Init();
	USART3_Init();
	//OLED_ShowChar(1, 2, '2');
	while(1)
	{
		
	}
}


//�����жϺ���
void USART3_IRQHandler(void)			 
{
	uint8_t Data; 
	uint8_t i;
	static uint8_t RxCounter1 = 0;
	static uint8_t RxBuffer1[7] = {0};//�����������
	static uint8_t RxState = 0;	      //���ݽ���״̬λ
	static uint8_t RxFlag = 0;

	if( USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)  	   //���������ж� 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);         //����жϱ�־λ
		Data = USART_ReceiveData(USART3);				       //��������
	
		if(RxState == 0 && Data == 0x2C)                       //0x2c֡ͷ��ʼ���ݽ��մ���
		{
			RxState=1;
			RxBuffer1[RxCounter1++] = Data;
		}

		else if(RxState==1&&Data==0x12)  //0x12֡ͷ
		{
			RxState=2;
			RxBuffer1[RxCounter1++] = Data;
		}

		else if(RxState==2)              //��ʽ��������
		{
			RxBuffer1[RxCounter1++] = Data;

			if(RxCounter1 >= 7 || Data == 0x5B)       //RxBuffer1�������ݽ���
			{
				RxState = 3;
				RxFlag = 1;
				length = (RxBuffer1[RxCounter1-4]<<8) + RxBuffer1[RxCounter1-5];
				Flag = (RxBuffer1[RxCounter1-2]<<8) + RxBuffer1[RxCounter1-3];			//�������Ǹ߰�λ		

			}
		}

		else if(RxState==3)		//������ݽ��ս�����־
		{
			if(RxBuffer1[RxCounter1-1] == 0x5B)
			{
				USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);//�رմ����ж�
				if(RxFlag == 1)
				{
					OLED_ShowNum(1, 1 ,length, 3);
					OLED_ShowNum(2, 1 ,Flag, 3);
				}
				RxCounter1 = 0;
				RxState = 0;
				USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//�е�û��ΪɶҪ�ȹ��ж�
			}
			else   //���մ���
			{
				RxState = 0;
				RxCounter1 = 0;
				for(i = 0;i < 7;i++)
				{
					RxBuffer1[i] = 0x00;      //���������������
				}
			}
		} 

		else   //�����쳣
		{
			RxState = 0;
			RxCounter1 = 0;
			for(i = 0;i < 7;i++)
			{
				RxBuffer1[i] = 0x00;      //���������������
			}
		}

	}
}







