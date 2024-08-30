#ifndef __LED_H__
#define __LED_H__

#include "stm32g4xx.h"                  // Device header
void LOCK_ON_OFF(void);
void LED_Init(void);
void LED_ON(uint8_t LEDx);
void LED_OFF(uint8_t LEDx);
void LED_TRG(uint8_t LEDx);
#endif
