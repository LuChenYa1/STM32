#include "ws2812b.h"
#include "tim.h"
#include "stdlib.h"

/*Some Static Colors------------------------------*/
const RGB_Color_TypeDef RED      = {255,0,0};  
const RGB_Color_TypeDef GREEN    = {0,255,0};  
const RGB_Color_TypeDef BLUE     = {0,0,255};  
const RGB_Color_TypeDef SKY      = {0,255,255};//深蓝
const RGB_Color_TypeDef MAGENTA  = {255,0,220};//天蓝
const RGB_Color_TypeDef YELLOW   = {128,216,0};//粉色
const RGB_Color_TypeDef OEANGE   = {127,106,0};//橘色
RGB_Color_TypeDef BLACK          = {0,0,0};//无颜色    
const RGB_Color_TypeDef WHITE    = {255,255,255};//白色

//好看的颜色封装成数组
RGB_Color_TypeDef table[16] = 
{
   {254,67,101}, 
   {76,0,10},
   {249,15,173},
   {128,0,32},
   {158,46,36},
   {184,206,142},
   {227,23,13},
   {178,34,34},
   {255,99,71},
   {99,38,18},
   {255,97,0},
   {21,161,201},
   {56,94,15},
   {50,205,50},
   {160,32,240},
    {218,60,90}
};

 //不封装
const RGB_Color_TypeDef color1 = {254,67,101};
//const RGB_Color_TypeDef color2 = {76,0,10};
//const RGB_Color_TypeDef color3 = {249,15,173};
//const RGB_Color_TypeDef color4 = {128,0,32};
//const RGB_Color_TypeDef color5 = {158,46,36};
//const RGB_Color_TypeDef color6 = {184,206,142};
//const RGB_Color_TypeDef color7 = {227,23,13};
//const RGB_Color_TypeDef color8 = {178,34,34};
//const RGB_Color_TypeDef color9 = {255,99,71};
//const RGB_Color_TypeDef color10 ={99,38,18};
//const RGB_Color_TypeDef color11= {255,97,0};
//const RGB_Color_TypeDef color12= {21,161,201};
//const RGB_Color_TypeDef color13= {56,94,15};
//const RGB_Color_TypeDef color14= {50,205,50};
//const RGB_Color_TypeDef color15= {160,32,240};
//const RGB_Color_TypeDef color16= {218,60,90};

 
//二维数据，装着每个灯的24位bit的RGB颜色数据所对应的占空比
uint32_t Pixel_Buf[Pixel_NUM+12][24]; 

//实验
uint32_t Pixel_Buf_1[48] = 
{
	CODE_1, CODE_0, CODE_0, CODE_1,CODE_1, CODE_0, CODE_0, CODE_1,
	CODE_1, CODE_0, CODE_0, CODE_1,CODE_1, CODE_0, CODE_0, CODE_1,
	CODE_1, CODE_0, CODE_0, CODE_1,CODE_1, CODE_0, CODE_0, CODE_1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0 ,0 ,0, 0, 0, 0, 0, 0,
	0 ,0 ,0, 0, 0, 0, 0, 0,	
};       

//输出24个占空比为零的PWM脉冲，作为最后复位延时
static void Reset_Load(void)
{
	uint8_t i, j;
	for(i = 0; i < 10; i++)
	{
		for(j = 0; j < 24; j++)
		{
			Pixel_Buf[Pixel_NUM + i][j] = 0;
		}
	}
}
//static void Reset_Load(void)
//{
//	uint8_t i, j;
//	for(i = 0; i < 1; i++)
//	{
//		for(j = 0; j < 24; j++)
//		{
//			Pixel_Buf[Pixel_NUM + i][j] = 0;
//		}
//	}
//}
 
//将写好的颜色数组对应的占空比数组发送到定时器3通道2进行输出
static void RGB_SendArray(void)
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)Pixel_Buf,(Pixel_NUM+10)*24);
//	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)Pixel_Buf_1,48);
}

//处理好占空比数组，然后发送
static void RGB_Flush(void)
{
    Reset_Load();     
    RGB_SendArray(); 
}

//一个灯的颜色数据译码为占空比
void RGB_SetOne_Color(uint8_t LEDId,RGB_Color_TypeDef Color)
{
	uint8_t i; 
	if(LEDId > Pixel_NUM)return; //避免越界
	//译码，将颜色的RGB数据例（255， 23， 125）转换为8bit * 3，数组装载对应的占空比
	for(i=0;i<8;i++) Pixel_Buf[LEDId][i]   = ( ((Color.G) & (1 << (7 -i)))? (CODE_1):CODE_0 );//G
	for(i=8;i<16;i++) Pixel_Buf[LEDId][i]  = ( ((Color.R) & (1 << (15-i)))? (CODE_1):CODE_0 );//R
	for(i=16;i<24;i++) Pixel_Buf[LEDId][i] = ( ((Color.B) & (1 << (23-i)))? (CODE_1):CODE_0 );//B
}

//连续对多个灯的颜色数据进行占空比转换，统一颜色
void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color)
{
    uint8_t i=0;
    for(i=head; i<=heal;i++)
    {
        RGB_SetOne_Color(i,color);
    }
}

//指定统一颜色
void RGB_Show_ALL(RGB_Color_TypeDef Color)   
{
    RGB_SetMore_Color(0, 30, Color);  
    RGB_Flush();      //WS2812B刷新显示
}

//单个灯显示
void RGB_Show_1_RED(void)   
{
    RGB_SetOne_Color(0,RED);  
    RGB_Flush();      //WS2812B刷新显示
}

//炫彩变化
void RGB_Show_30(void)   
{
    RGB_SetMore_Color(0,30,BLACK);  //全暗
    RGB_SetMore_Color(0,rand()%10,table[rand()%16]);    //随机颜色
    RGB_SetMore_Color(10,rand()%10+11,table[rand()%16]);  //不同段不同颜色
	RGB_SetMore_Color(22,rand()%10+8,table[rand()%16]);  //不同段不同颜色
//    RGB_SetMore_Color(16,rand()%8+16,table[rand()%16]);
//    RGB_SetMore_Color(24,rand()%8+24,table[rand()%16]);
    
    RGB_Flush();      //WS2812B刷新显示
}



