#ifndef __WS2812B_H__
#define __WS2812B_H__
 
#include "main.h"
 
//0码和1码对应的PWM波占空比
#define CODE_1       (66)       //66 / 99 = 66%
#define CODE_0       (33)       //33 / 99 = 33%
 
//单个LED的RGB结构体
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_Color_TypeDef;

extern  RGB_Color_TypeDef BLACK;//无颜色    

extern RGB_Color_TypeDef table[16];

#define Pixel_NUM 30  //LED灯带数量

static void Reset_Load(void); //将最后24个数据变成0，代表RESET_code

static void RGB_SendArray(void);//发送最终数组

static void RGB_Flush(void);  //刷新RGB显示

void RGB_SetOne_Color(uint8_t LEDId,RGB_Color_TypeDef Color);//给单个LED装载24位bit颜色数据

void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color);//多个LED显示相同颜色

void RGB_Show_21(void); //RGB写入函数

void RGB_Show_ALL(RGB_Color_TypeDef Color);  

#endif


