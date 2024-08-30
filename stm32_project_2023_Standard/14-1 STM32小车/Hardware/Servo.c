#include "stm32f10x.h"                  // Device header
#include "PWM_Servo.h"                  // Device header
#include "Delay.h"                  // Device header

//继承PWM，中间层应用

#define Time 300

//初始化
void Servo_Init(void)
{
	PWMServo_Init();
	Delay_ms(Time);
}

//左转90度
void Servo_left90(void)
{
	PWMServo_SetCompare3(2500);
	Delay_ms(Time);
}

//左转45度
void Servo_left45(void)
{
	PWMServo_SetCompare3(2000);
	Delay_ms(Time);
}

//不转
void Servo_middle0(void)
{
	PWMServo_SetCompare3(1500);
	Delay_ms(Time);
}

//右转45度
void Servo_right45(void)
{
	PWMServo_SetCompare3(1000);
	Delay_ms(Time);
}

//右转90度
void Servo_right90(void)
{
	PWMServo_SetCompare3(500);
	Delay_ms(Time);
}
