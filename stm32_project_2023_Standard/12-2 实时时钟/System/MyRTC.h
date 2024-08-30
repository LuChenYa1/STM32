#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f10x.h"                  // Device header

extern uint16_t MyRTC_Time[6];
void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif
