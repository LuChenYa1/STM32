#include "stm32f10x.h"                  // Device header
#include "PWM.h"

//继承PWM，中间层应用
//驱动电机
/**
  * @brief  初始化电机，总使能PA3，分开关PA4、PA5，输出比较通道3用于输出PWM
  * @param  无
  * @retval 无
  */
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init();
}


/**
  * @brief  设置电机速度（ -100 ~ +100 ）
  * @param  
  * @retval 
  */
void Motor_SetSpeed(int8_t Speed)
{
	if(Speed >= 0)//正转
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7); 
		PWM_SetCompare3(Speed);
	}
	else//反转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		PWM_SetCompare3(-Speed); 		
	}
}
