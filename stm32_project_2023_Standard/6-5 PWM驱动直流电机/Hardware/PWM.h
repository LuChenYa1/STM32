#ifndef __PWM_H__
#define __PWM_H__
#include "stm32f10x.h"                  // Device header

void PWM_Init(void);
void PWM_SetCompare3(uint8_t Compare);

#endif
