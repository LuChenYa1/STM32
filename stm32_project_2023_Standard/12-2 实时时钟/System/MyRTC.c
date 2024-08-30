#include "stm32f10x.h"                  // Device header
#include <Time.h>

uint16_t MyRTC_Time[6] = {2024, 1, 19, 14, 50, 40};//16进制数的个位数不要前补0，C语言中以0为前缀的数是八进制数

void MyRTC_SetTime(void);

void MyRTC_Init(void)
{
	//未完全断电时，RTC时钟一直工作
	//计数器一直自增，同时证明时钟源也一直在工作，不管是否复位
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);    //使能PWR和BKP
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);                           //设置PWR的DBP
	
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)          //只在初始上电时执行if语句，此后不完全断电则不再执行该语句        
	{                                        
		//利用BKP寄存器不完全断电时数据不丢失的特性，防止一旦复位时间回到原点
		//但该方法只适用于未完全断电的情况（主电源和备用电源）
		
		RCC_LSEConfig(RCC_LSE_ON);                         //启动LSE时钟
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//等待时钟启动完成

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);            //选择低速振荡器时钟作为时钟源（32.768KHZ）
		RCC_RTCCLKCmd(ENABLE);                             //使能RTC时钟

		RTC_WaitForLastTask();                             //等待上一次写入操作完成，当然，这里其实还没开始写入寄存器，可以不用写
		RTC_WaitForSynchro();                              //等待同步，总线的PCLK和RTCCLK不一样，需要对齐边沿

		RTC_SetPrescaler(32768-1);                         //RTCCLK时钟32768分频后为1HZ
		RTC_WaitForLastTask();                             //等待写操作完成

//	    RTC_SetCounter(1672588795);                        //设置初始计数值，相当于设置计时初始时间
//	    RTC_WaitForLastTask();                             //等待写操作完成

		MyRTC_SetTime();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	
}


void MyRTC_SetTime(void)
{
	time_t Time_Cnt;    //秒计数值
	struct tm Time_Date;//存放年月日时分秒的结构体
	
	Time_Date.tm_year = MyRTC_Time[0] - 1900;
	Time_Date.tm_mon = MyRTC_Time[1] - 1 ;
	Time_Date.tm_mday = MyRTC_Time[2];
	Time_Date.tm_hour = MyRTC_Time[3];
	Time_Date.tm_min = MyRTC_Time[4];
	Time_Date.tm_sec = MyRTC_Time[5];
	
	Time_Cnt = mktime(&Time_Date) - 8 * 60 * 60;//日期转换秒计数值，北京时间转伦敦时间
	
	RTC_SetCounter(Time_Cnt);
	RTC_WaitForLastTask();
}


void MyRTC_ReadTime(void)
{
	time_t Time_Cnt;    //秒计数值
	struct tm Time_Date;//存放年月日时分秒的结构体
	
	Time_Cnt = RTC_GetCounter() + 8 * 60 * 60;//伦敦时间转北京时间
	Time_Date = *localtime(&Time_Cnt);
	
	MyRTC_Time[0] = Time_Date.tm_year + 1900;
	MyRTC_Time[1] = Time_Date.tm_mon+ 1 ;
	MyRTC_Time[2] = Time_Date.tm_mday;
	MyRTC_Time[3] = Time_Date.tm_hour;
	MyRTC_Time[4] = Time_Date.tm_min;
	MyRTC_Time[5] = Time_Date.tm_sec;
}
