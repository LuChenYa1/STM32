#ifndef __UART_IO_H__
#define __UART_IO_H__

#include "stm32g4xx_hal.h"

void USART_IO_SendByte(uint8_t data);
uint8_t USART_IO_ReceiveByte(void);
void USART_IO_SendString(const char *str);
void USART_IO_ReceiveString(char *str, uint16_t size);

#endif
