#ifndef __WS2812B_H__
#define __WS2812B_H__
 
#include "main.h"
 
//0���1���Ӧ��PWM��ռ�ձ�
#define CODE_1       (66)       //66 / 99 = 66%
#define CODE_0       (33)       //33 / 99 = 33%
 
//����LED��RGB�ṹ��
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_Color_TypeDef;

extern  RGB_Color_TypeDef BLACK;//����ɫ    

extern RGB_Color_TypeDef table[16];

#define Pixel_NUM 30  //LED�ƴ�����

static void Reset_Load(void); //�����24�����ݱ��0������RESET_code

static void RGB_SendArray(void);//������������

static void RGB_Flush(void);  //ˢ��RGB��ʾ

void RGB_SetOne_Color(uint8_t LEDId,RGB_Color_TypeDef Color);//������LEDװ��24λbit��ɫ����

void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color);//���LED��ʾ��ͬ��ɫ

void RGB_Show_21(void); //RGBд�뺯��

void RGB_Show_ALL(RGB_Color_TypeDef Color);  

#endif


