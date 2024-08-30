#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "stm32f10x.h"                  // Device header

void Encoder_Init(void);
int16_t Encoder_Get(void);

#endif
