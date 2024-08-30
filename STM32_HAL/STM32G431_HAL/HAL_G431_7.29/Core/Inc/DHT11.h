#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

#define	DHT11_PIN_PORT       GPIOB
#define	DHT11_PIN        GPIO_PIN_5
#define	DHT11_PIN_CLOCK  __HAL_RCC_GPIOA_CLK_ENABLE()
#define	DHT11_PIN_OUT_H   HAL_GPIO_WritePin(DHT11_PIN_PORT, DHT11_PIN, GPIO_PIN_SET)
#define	DHT11_PIN_OUT_L   HAL_GPIO_WritePin(DHT11_PIN_PORT, DHT11_PIN, GPIO_PIN_RESET)
#define	DHT11_PIN_IN      HAL_GPIO_ReadPin(DHT11_PIN_PORT, DHT11_PIN)


uint8_t DHT11_Init(void);//ԵʼۯDHT11
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi); //ׁȡςʪ׈
uint8_t DHT11_Read_Byte(void);//ׁԶһٶؖޚ
uint8_t DHT11_Read_Bit(void);//ׁԶһٶλ
uint8_t DHT11_Check(void);//ݬӢˇرզ՚DHT11
void DHT11_Rst(void);//شλDHT11


#endif
