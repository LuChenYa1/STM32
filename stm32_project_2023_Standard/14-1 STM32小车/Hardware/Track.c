#include "stm32f10x.h"                  // Device header
#include "Car.h"

#define Right   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)
#define Centure GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)
#define Left    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)

#define BLACK 1
#define WHITE 0


void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 
}
void XunJi(void)
{
	//直线
	if(Right == WHITE && Centure == BLACK && Left == WHITE) 
	{
		Car_GoAhead();
	}       
	else if(Right == BLACK && Centure == WHITE && Left == BLACK)
	{
		Car_GoAhead(); 
	}		
	
	/****************************************************/
	//左偏右拐
	else if(Right == BLACK && Centure == WHITE && Left == WHITE)
	{
		Car_TurnRight();
		if(Right == WHITE && Centure == WHITE && Left == WHITE)	Car_GoAhead();
	}		
	else if(Right == BLACK && Centure == BLACK && Left == WHITE)
	{
		Car_TurnRight();
	    if(Right == WHITE && Centure == WHITE && Left == WHITE)	Car_GoAhead();
	}		
	/****************************************************/
	//右偏左拐
	else if(Right == WHITE && Centure == WHITE && Left == BLACK)
	{
		Car_TurnLeft();
		if(Right == WHITE && Centure == WHITE && Left == WHITE)	Car_GoAhead();
	}		 
	else if(Right == WHITE && Centure == BLACK && Left == BLACK)
	{	
		Car_TurnLeft();
		if(Right == WHITE && Centure == WHITE && Left == WHITE)	Car_GoAhead();
	}		 
	/***************************************************/
	//无法直角转弯
	else if(Right == WHITE&&Centure == WHITE&&Left == WHITE)Car_Stop(); //停止
}
