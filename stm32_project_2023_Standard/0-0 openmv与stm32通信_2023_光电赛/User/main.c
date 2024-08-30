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


//串口中断函数
void USART3_IRQHandler(void)			 
{
	uint8_t Data; 
	uint8_t i;
	static uint8_t RxCounter1 = 0;
	static uint8_t RxBuffer1[7] = {0};//存放数据数组
	static uint8_t RxState = 0;	      //数据接收状态位
	static uint8_t RxFlag = 0;

	if( USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)  	   //触发接收中断 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);         //清除中断标志位
		Data = USART_ReceiveData(USART3);				       //接收数据
	
		if(RxState == 0 && Data == 0x2C)                       //0x2c帧头开始数据接收处理
		{
			RxState=1;
			RxBuffer1[RxCounter1++] = Data;
		}

		else if(RxState==1&&Data==0x12)  //0x12帧头
		{
			RxState=2;
			RxBuffer1[RxCounter1++] = Data;
		}

		else if(RxState==2)              //正式接收数据
		{
			RxBuffer1[RxCounter1++] = Data;

			if(RxCounter1 >= 7 || Data == 0x5B)       //RxBuffer1接收数据结束
			{
				RxState = 3;
				RxFlag = 1;
				length = (RxBuffer1[RxCounter1-4]<<8) + RxBuffer1[RxCounter1-5];
				Flag = (RxBuffer1[RxCounter1-2]<<8) + RxBuffer1[RxCounter1-3];			//后来者是高八位		

			}
		}

		else if(RxState==3)		//检测数据接收结束标志
		{
			if(RxBuffer1[RxCounter1-1] == 0x5B)
			{
				USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);//关闭串口中断
				if(RxFlag == 1)
				{
					OLED_ShowNum(1, 1 ,length, 3);
					OLED_ShowNum(2, 1 ,Flag, 3);
				}
				RxCounter1 = 0;
				RxState = 0;
				USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//有点没懂为啥要先关中断
			}
			else   //接收错误
			{
				RxState = 0;
				RxCounter1 = 0;
				for(i = 0;i < 7;i++)
				{
					RxBuffer1[i] = 0x00;      //存放数据数组清零
				}
			}
		} 

		else   //接收异常
		{
			RxState = 0;
			RxCounter1 = 0;
			for(i = 0;i < 7;i++)
			{
				RxBuffer1[i] = 0x00;      //存放数据数组清零
			}
		}

	}
}







