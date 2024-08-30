#ifndef __AD_H__
#define __AD_H__
#include "stm32f10x.h"                  // Device header

extern uint16_t AD_Value[4];
void AD_Init(void);
void ADC_GetValue(void);

#endif
