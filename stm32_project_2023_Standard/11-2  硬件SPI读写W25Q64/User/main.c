#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "W25Q64.h"                  // Device header

uint8_t MID;
uint16_t DID;
uint8_t ArrayWrite[4] = {0x02, 0x04, 0x06, 0x08};
uint8_t ArrayRead[4];

int main(void)
{
	OLED_Init();
	 
	W25Q64_Init();

	W25Q64_ReadID(&MID, &DID);
	
	OLED_ShowString(1, 1, "MID:");
	OLED_ShowString(1, 8, "DID:");
	OLED_ShowString(2, 1, "W:");
	OLED_ShowString(3, 1, "R:");
	
	OLED_ShowHexNum(1, 5, MID, 2);
	OLED_ShowHexNum(1, 12, DID, 4);
	
	//写入数据前必须擦除使内部数据全为1，因为W25Q64有无法写1的特性，写数据前不擦除会写入错误数据
	W25Q64_SectorErase(0x000000);//扇区起始地址最后三位都是0
	W25Q64_PageProgram(0x000000, ArrayWrite, 4);//写入地址不能跨页，页范围是0xXXXX00 ~ 0xXXXXFF，超范围会回到页的最开始进行写入
	W25Q64_ReadData(0x000000, ArrayRead, 4);//掉电不丢失
	
	OLED_ShowHexNum(2, 3, ArrayWrite[0], 2);
	OLED_ShowHexNum(2, 6, ArrayWrite[1], 2);
	OLED_ShowHexNum(2, 9, ArrayWrite[2], 2);
	OLED_ShowHexNum(2, 12, ArrayWrite[3], 2);
	
	OLED_ShowHexNum(3, 3, ArrayRead[0], 2);
	OLED_ShowHexNum(3, 6, ArrayRead[1], 2);
	OLED_ShowHexNum(3, 9, ArrayRead[2], 2);
	OLED_ShowHexNum(3, 12, ArrayWrite[3], 2);
	
	while (1)
	{
		
	}
}
