#ifndef __IC_H__
#define __IC_H__
#include "stm32f10x.h"                  // Device header

void IC_Init(void);
uint32_t IC_GetFreq(void);
uint32_t IC_GetDuty(void);


#endif
