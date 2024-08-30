#ifndef __SEG_H__
#define __SEG_H__
#include "main.h"
void Seg_Display(uint8_t byte1, uint8_t byte2, uint8_t byte3);

#define SER_Port GPIOA
#define SER_Pin GPIO_PIN_1
#define RCK_Port GPIOA
#define RCK_Pin GPIO_PIN_2
#define SCK_Port GPIOA
#define SCK_Pin GPIO_PIN_3

#define RCK_H() HAL_GPIO_WritePin(RCK_Port, RCK_Pin, GPIO_PIN_SET)
#define RCK_L() HAL_GPIO_WritePin(RCK_Port, RCK_Pin, GPIO_PIN_RESET)
#define SCK_H() HAL_GPIO_WritePin(SCK_Port, SCK_Pin, GPIO_PIN_SET)
#define SCK_L() HAL_GPIO_WritePin(SCK_Port, SCK_Pin, GPIO_PIN_RESET)
#define SER_H() HAL_GPIO_WritePin(SER_Port, SER_Pin, GPIO_PIN_SET)
#define SER_L() HAL_GPIO_WritePin(SER_Port, SER_Pin, GPIO_PIN_RESET)

#endif
