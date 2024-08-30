#ifndef __PWMSERVO_H__
#define __PWMSERVO_H__
#include "stm32f10x.h"                  // Device header

void PWMServo_Init(void);
void PWMServo_SetCompare3(uint16_t Compare);

#endif
