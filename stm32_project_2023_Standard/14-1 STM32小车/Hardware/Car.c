#include "stm32f10x.h"                  // Device header
#include "Motor.h"                  // Device header


void Car_Init(void)
{
	Motor_Init();
}

//停止
void Car_Stop(void)
{
	Motor_SetLeftSpeed(0);
	Motor_SetRightSpeed(0);
}
//向前直走
void Car_GoAhead(void)
{
	Motor_SetLeftSpeed(90);
	Motor_SetRightSpeed(90);
}

//向后直走
void Car_GoBack(void)
{
	Motor_SetLeftSpeed(-90);
	Motor_SetRightSpeed(-90);
}

//向前左转
void Car_TurnLeft(void)
{
	Motor_SetLeftSpeed(-50);
	Motor_SetRightSpeed(90);
}

//向前右转
void Car_TurnRight(void)
{
	Motor_SetLeftSpeed(90);
	Motor_SetRightSpeed(-50);
}

//原地向后左转
void Car_SelfLeft(void)
{
	Motor_SetLeftSpeed(-90);
	Motor_SetRightSpeed(90);
}

//原地向后右转
void Car_SelfRight(void)
{
	Motor_SetLeftSpeed(90);
	Motor_SetRightSpeed(-90);
}
