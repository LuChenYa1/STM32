#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "stm32f10x.h"                  // Device header
#include <stdio.h>

extern uint8_t Serial_RxData;
extern uint8_t Serial_RxFlag;
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t array[], uint16_t Length);
void Serial_SendString(uint8_t *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);

void Serial_Printf(uint8_t *format, ...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);


#endif
