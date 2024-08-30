#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm32f10x.h"                  // Device header

void Motor_Init(void);
void Motor_SetLeftSpeed(int16_t Speed);
void Motor_SetRightSpeed(int16_t Speed);

#endif
