#ifndef __W25Q64_H__
#define __W25Q64_H__
#include "stm32f10x.h"                  // Device header

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID);//用指针方式返回两个数据
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count);//三个字节地址，要存储的数据， 数据的个数(最多256个)
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);//三个字节地址，读取到的数据所存放的数组，数据的个数（无限制）

#endif
