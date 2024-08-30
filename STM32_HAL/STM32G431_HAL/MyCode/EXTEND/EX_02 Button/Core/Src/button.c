#include "button.h"

uint8_t Button_Scan(void)
{
	uint16_t adc = getADC_R37();
	if(adc < 0x0fff / 14 * 1)return 1;
	else if(adc > (0x0fff / 14 * 1) && adc < 0x0fff / 14 * 3)return 2;
	else if(adc > (0x0fff / 14 * 3) && adc < 0x0fff / 14 *5)return 3;
	else if(adc > (0x0fff / 14 * 5) && adc < 0x0fff / 14 * 7)return 4;
	else if(adc > (0x0fff / 14 * 7) && adc < 0x0fff / 14 * 9)return 5;
	else if(adc > (0x0fff / 14 * 9) && adc < 0x0fff / 14 * 11)return 6;
	else if(adc > (0x0fff / 14 * 11) && adc < 0x0fff / 14 * 13)return 7;
	else if(adc > (0x0fff / 14 * 13) && adc < 0x0f6f)return 8;
	else return 0;
}
