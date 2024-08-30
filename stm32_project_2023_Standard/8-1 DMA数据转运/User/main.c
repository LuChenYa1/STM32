#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "LED.h"
#include "Key.h"
#include "Stdint.h"
#include "OLED.h"
#include "MyDMA.h"

uint8_t DataA[4] = {0x01, 0x02, 0x03, 0x04};
uint8_t DataB[4] = {0};

int main(void)
{
	OLED_Init();
	MyDMA_Init((uint32_t)DataA, (uint32_t)DataB, 4);
	
	//OLED_ShowHexNum(1, 1, (uint32_t)&ADC1->DR, 8);
	//ADC1一开始是外设ADC1的首地址的宏定义，后来被强制转换为结构体指针
	//该结构体含有uint32_t类型的一堆变量，都以ADC1的所有内部设施为名，且变量顺序和ADC1的内部设施的首地址顺序一致
	//巧妙地利用结构体特性，通过对结构体成员取地址完成了对外设及其寄存器地址地查询
	
	 
	OLED_ShowString(1, 1, "DataA");
	OLED_ShowString(3, 1, "DataB");
	OLED_ShowHexNum(1, 8, (uint32_t)&DataA, 8);
	OLED_ShowHexNum(3, 8, (uint32_t)&DataB, 8);
	
	
	while(1)
	{
		 for(int i = 0;i < 4;i++)
		{
			DataA[i]++;
		}
		//显示没转换时数组的值
		OLED_ShowHexNum(2, 1, DataA[0], 2);
		OLED_ShowHexNum(2, 4, DataA[1], 2);
		OLED_ShowHexNum(2, 7, DataA[2], 2);
		OLED_ShowHexNum(2, 10, DataA[3], 2);

		OLED_ShowHexNum(4, 1, DataB[0], 2);
		OLED_ShowHexNum(4, 4, DataB[1], 2);
		OLED_ShowHexNum(4, 7, DataB[2], 2);
		OLED_ShowHexNum(4, 10, DataB[3], 2);
		
		Delay_ms(1000);//留影
		
		MyDMA_Transfer();
		
		//显示转换后数组的值
		OLED_ShowHexNum(2, 1, DataA[0], 2);
		OLED_ShowHexNum(2, 4, DataA[1], 2);
		OLED_ShowHexNum(2, 7, DataA[2], 2);
		OLED_ShowHexNum(2, 10, DataA[3], 2);

		OLED_ShowHexNum(4, 1, DataB[0], 2);
		OLED_ShowHexNum(4, 4, DataB[1], 2);
		OLED_ShowHexNum(4, 7, DataB[2], 2);
		OLED_ShowHexNum(4, 10, DataB[3], 2);
		
		Delay_ms(1000);//留影
	}
}
