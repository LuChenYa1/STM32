#ifndef __MPU6050_H__
#define __MPU6050_H__
#include "stm32f10x.h"                  // Device header

void MPU6050_Init(void);
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);//寄存器地址Register Address
uint8_t MPU6050_ReceiveReg(uint8_t RegAddress);

void MPU6050_ReceiveRegArray(uint8_t RegAddress, uint8_t *DataArray);

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
					 int16_t *Temp,
					 int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
uint8_t MPU6050_GetID(void);


#endif
