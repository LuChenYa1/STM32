#include "stm32f10x.h"                  // Device header
#include "PWM.h"                  // Device header

//继承PWM，中间层应用

//初始化
void Servo_init(void)
{
	PWM_Init();
}

//左转90度
void Servo_left90(void)
{
	PWM_SetCompare2(2500);
}

//左转45度
void Servo_left45(void)
{
	PWM_SetCompare2(2000);
}

//不转
void Servo_middle0(void)
{
	PWM_SetCompare2(1500);
}

//右转45度
void Servo_right45(void)
{
	PWM_SetCompare2(1000);
}

//右转90度
void Servo_right90(void)
{
	PWM_SetCompare2(500);
}
