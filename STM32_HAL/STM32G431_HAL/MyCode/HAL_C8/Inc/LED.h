#ifndef __LED_H__
#define __LED_H__

//void LED_Tog(uint8_t LEDx);
//void LED_OFF(uint8_t LEDx);
//void LED_ON(uint8_t LEDx);
//void LOCK_ON_OFF();
//void LED_Init(void);

#include "stm32g4xx.h"                  // Device header
void LED_Init(void);
void LED_ON(uint8_t LEDx);
void LED_OFF(uint8_t LEDx);
void LED_TRG(uint8_t LEDx);

#endif
