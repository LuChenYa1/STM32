#include "Seg.h"

uint8_t Seg7[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
					0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};
					
void Seg_Display(uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
	uint8_t i = 0, code;
	RCK_L();
	
	code = Seg7[byte3];
	for(i = 0; i < 8; i++)
	{
		SCK_L();
		if(code & (0x80 >> i))SER_H();
		else SER_L();
		SCK_H();
	}
	code = Seg7[byte2];
	for(i = 0; i < 8; i++)
	{
		SCK_L();
		if(code & (0x80 >> i))SER_H();
		else SER_L();
		SCK_H();
	}
	code = Seg7[byte1];
	for(i = 0; i < 8; i++)
	{
		SCK_L();
		if(code & (0x80 >> i))SER_H();
		else SER_L();
		SCK_H();
	}
	RCK_H();
}
