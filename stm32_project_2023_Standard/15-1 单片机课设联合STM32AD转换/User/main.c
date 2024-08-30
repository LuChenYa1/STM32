#include "stm32f10x.h"  // Device header              
#include "Stdint.h"
#include "OLED.h"
#include "AD.h"

uint16_t ADValue;
uint8_t Humidity_Past, Humidity_Now, Humidity_Limit;
void Humidity_Init(void);

int main(void)
{
	AD_Init();
	OLED_Init();
	Humidity_Init();
	
	Humidity_Limit = 40;
	
	OLED_ShowString(1, 1, "ADValue:");
	OLED_ShowString(2, 1, "Humidity:00");
	while(1)
	{
		ADValue = ADC_GetValue();
		Humidity_Past = Humidity_Now;
		Humidity_Now = (float)ADValue / 4095 * 100;//0x00~0x63
		
		OLED_ShowNum(1, 9, ADValue, 4);//第一行显示未转换值
		if(Humidity_Past != Humidity_Now)
		{
			OLED_ShowNum(2, 12, Humidity_Now, 2);//第二行显示转换值    
		}			
		
		if(Humidity_Now > Humidity_Limit)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_2); 
			OLED_ShowString(4, 1, "Dry ");
			OLED_ShowNum(4, 16, GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2),1);
		}
		else 
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_2); 
			OLED_ShowString(4, 1, "Rain");
			OLED_ShowNum(4, 16, GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2),1);
		}
	}
}

void Humidity_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//初始默认低电平
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//数据跳变是正常现象，但使用看门狗中断，利用阈值完成例如开关灯操作时，会出现灯闪的不良现象
//解决方法：施密特滤波；均值滤波；左对齐减小分辨率（精度），把数据的尾数去掉
//由于ADC内部硬件问题，ADValue最高时不能达到3.3V
