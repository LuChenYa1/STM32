#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "Stdint.h"
#include "OLED.h"
#include "CountSensor.h"

//����ʱ����������Pin14����ߵ�ƽ���ɿ����Ϊ�͵�ƽ�������½����жϣ��жϱ�־λ��һ��ִ���жϳ��򣬱�־λ����

int main(void)
{
	OLED_Init();
	CountSensor_Init();
	
	OLED_ShowString(1,1,"Count:");
	
	while(1)
	{
		OLED_ShowNum(1,7,Get_SensorCounter(),4);
	}
}

