#include "stm32f10x.h"  // Device header
#include "Delay.h"               
#include "OLED.h"
#include "Serial.h"
#include "Car.h"
#include "Servo.h"
#include "ChaoShengBo.h"
#include "Track.h"
#include "Motor.h"                   

uint8_t Flag_XunJi_Bizhang_Serial = 0x00;

uint16_t distance, distance_left, distance_right;

#define Targetdistance  30

void Car_Show(void);
void Track_left_right(void);
void BiZhang(void);
void Follow(void);

int main(void)
{
	OLED_Init();
	Car_Init();
	Serial_Init();
	Servo_Init();
	ChaoShengBo_Init();
	
	Servo_middle0();//确保初始状态回正
	
	
	while(1)
	{	
		Servo_left90();
		Delay_ms(200);
		Servo_right45();
		Delay_ms(200);
//		if(Flag_XunJi_Bizhang_Serial == 0x01)BiZhang();
//		else if(Flag_XunJi_Bizhang_Serial == 0x02)XunJi();
//		else if(Flag_XunJi_Bizhang_Serial == 0x04)Follow();
//		
//		OLED_ShowNum(3, 1, Flag_XunJi_Bizhang_Serial, 1);
	}
}


void Follow(void)
{
	distance = Track_Distance() * 0.017;
	Track_left_right();//左右转头测距
	if(distance < distance_left && distance < distance_right && distance > 10)//正前方有物体
	{
		Car_GoAhead();
	}
	else if(distance_left < distance && distance_left < distance_right )//左边有物体
	{
		if(distance_left > 10)
		{
			Car_SelfLeft();
			Delay_ms(1600);
			Car_GoAhead();
		}
		else
		{
			Car_Stop();
		}
	}
	else if(distance_right < distance_left && distance_right < distance )//右边有物体 
	{
		if(distance_right > 10)
		{
			Car_SelfRight();
			Delay_ms(1600);
			Car_GoAhead();
		}
		else
		{
			Car_Stop();
		}
	}
	 
}

void BiZhang(void)
{
	//单一避障
	Car_GoAhead();
	distance = Track_Distance() * 0.017;
	if(distance < Targetdistance)//前方有障碍物
	{
		Car_Stop();
		Delay_ms(60);
		
		Track_left_right();//左右转头测距
		if(distance_left > distance_right && distance_left > Targetdistance)//左边畅通
		{
			Car_SelfLeft();
			Delay_ms(350);
			Car_Stop();
			Delay_ms(60);
		}
		else if(distance_right > distance_left && distance_right > Targetdistance)//右边畅通
		{
			Car_SelfRight();
			Delay_ms(350);
			Car_Stop();
			Delay_ms(60);
		}
		else if(distance_left < Targetdistance && distance_right < Targetdistance)//全部堵塞
		{
			Servo_middle0();
			Car_Stop();
			Delay_ms(200);
			Car_SelfRight();//原地向后转
			Delay_ms(1500);
		}
	}
//	//避障 + 循迹
//	distance = Track_Distance() * 0.017;
//	if(distance < Targetdistance)//前方有障碍物
//	{
//		Car_Stop();
//		Delay_ms(1000);
//		Flag_Serial_xunji = 1;//有障碍物，暂停循迹，通过蓝牙发送11重新开始循迹
//	}
//	else
//	{
//		
//	}
}

void Car_Show(void)
{
	OLED_ShowHexNum(1, 8, Serial_RxData, 2); 
	OLED_ShowNum(4, 1, Flag_XunJi_Bizhang_Serial, 1);
	if(Serial_RxData == 0x00)OLED_ShowString(2, 1, "Stop");
	else if(Serial_RxData == 0x01)OLED_ShowString(2, 1, "Go ahead");
	else if(Serial_RxData == 0x02)OLED_ShowString(2, 1, "Go back");
	else if(Serial_RxData == 0x03)OLED_ShowString(2, 1, "Turn left");
	else if(Serial_RxData == 0x04)OLED_ShowString(2, 1, "Turn right");
	
	else if(Serial_RxData == 0x06)OLED_ShowString(2, 1, "Self right");
	else if(Serial_RxData == 0x07)OLED_ShowString(2, 1, "left45");
	else if(Serial_RxData == 0x08)OLED_ShowString(2, 1, "right45");
	
	else if(Serial_RxData == 0x09)OLED_ShowString(2, 1, "BiZhang");
	else if(Serial_RxData == 0x0A)OLED_ShowString(2, 1, "XunJi");
	else if(Serial_RxData == 0x0B)OLED_ShowString(2, 1, "LanYa");
}


//USART1的接收数据中断
void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		
		if(Serial_RxData == 0x09)//该句不能放到主函数里，会让模式一直切换
		{
			Flag_XunJi_Bizhang_Serial = 0x01;//开启避障功能
		}
		 
		else if(Serial_RxData == 0x0A)
		{
			Flag_XunJi_Bizhang_Serial = 0x02;//开启循迹
		}
		else if(Serial_RxData == 0x0B)//开启蓝牙遥控
		{
		    Flag_XunJi_Bizhang_Serial = 0x03;
		}
		else if(Serial_RxData == 0x0C)
		{
		    Flag_XunJi_Bizhang_Serial = 0x04;			
		}
		if(Flag_XunJi_Bizhang_Serial == 0x03)
		{
			if(Serial_RxData == 0x00)Car_Stop();
			else if(Serial_RxData == 0x01)Car_GoAhead();
			else if(Serial_RxData == 0x02)Car_GoBack();
			else if(Serial_RxData == 0x03)Car_TurnLeft();
			else if(Serial_RxData == 0x04)Car_TurnRight();
			
			else if(Serial_RxData == 0x06)Car_SelfRight();
			else if(Serial_RxData == 0x07)Servo_left45();
			else if(Serial_RxData == 0x08)Servo_right45();
		}
		
		//Car_Show(); 
		
		USART_ClearITPendingBit(USART1, USART_FLAG_RXNE);//清除哪个外设的哪个标志位,RXNE有读操作就清零，该句在该前提下可以不写
	}
}

void Track_left_right(void)
{
	//左转45
	Servo_left45();//函数自带延时
	distance_left = Track_Distance() * 0.017;
	Servo_right45();//右转45
	distance_right = Track_Distance() * 0.017;
	Servo_middle0();//回正
}
