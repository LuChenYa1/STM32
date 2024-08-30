#include "seg.h"

uint8_t Seg_Code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x3f};

void Seg_Display(uint8_t Bit1, uint8_t Bit2, uint8_t Bit3)
{
	uint8_t i, Code;
	RCK_L;
	
	Code = Seg_Code[Bit3];
	for(i = 0; i < 8; i++)
	{
		SCK_L;
		if(Code &(0x80 >> i))SER_H;
		else SER_L;
		SCK_H;
	}
	Code = Seg_Code[Bit2];
	for(i = 0; i < 8; i++)
	{
		SCK_L;
		if(Code &(0x80 >> i))SER_H;
		else SER_L;
		SCK_H;
	}
	Code = Seg_Code[Bit1];
	for(i = 0; i < 8; i++)
	{
		SCK_L;
		if(Code &(0x80 >> i))SER_H;
		else SER_L;
		SCK_H;
	}
	RCK_H;
}
