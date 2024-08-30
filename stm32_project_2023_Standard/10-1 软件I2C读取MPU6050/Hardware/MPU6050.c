#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"


#define MPU6050_ADDRESS 0xD0  //(写模式)


/**
  * @brief  向指定外设的寄存器写入数据
  * @param  RegAddress：寄存器地址
  * @param  Data：要写入的数据
  * @retval 无
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)//寄存器地址Register Address
{
	//注意：每读或写一个字节，寄存器地址指针都会后移一位，指向新的寄存器
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);// 1101 0000     =    MPU6050的地址 + 写模式
	MyI2C_ReceiveACK();             //这里接收返回值已省略
	MyI2C_SendByte(RegAddress);     //指定要操作的寄存器地址   
	MyI2C_ReceiveACK();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveACK();
	MyI2C_Stop();
}



/**
  * @brief  向指定外设的寄存器读取数据
  * @param  RegAddress：寄存器地址
  * @retval Data：读取的数据
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);// 1101 0000     =    MPU6050的地址 + 写模式
	MyI2C_ReceiveACK();             //这里接收返回值已省略
	MyI2C_SendByte(RegAddress);     //指定地址指针在该寄存器位置，重新起始时，自动对该寄存器进行读写操作 
	MyI2C_ReceiveACK();
	
	//重复起始条件
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
	MyI2C_ReceiveACK();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendACK(1);//发送非应答，停止接收数据
	MyI2C_Stop();
	
	return Data;
}



/**
  * @brief  对MPU6050连续读取16个字节
  * @param  RegAddress：寄存器地址
  * @param  DataArray：存放数据的数组，需要后期重组
  * @retval 无
  */
void MPU6050_ReceiveRegArray(uint8_t RegAddress, uint8_t *DataArray)
{
	uint8_t i;
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);       // 1101 0000     =    MPU6050的地址 + 写模式
	MyI2C_ReceiveACK();                    //这里接收返回值已省略
	MyI2C_SendByte(RegAddress);            //指定地址指针在该寄存器位置，重新起始时，自动对该寄存器进行读写操作 
	MyI2C_ReceiveACK();
	
	//重复起始条件
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//发送读命令
	MyI2C_ReceiveACK();
	for(i = 0; i < 15;i++)
	{
		DataArray[i] = MyI2C_ReceiveByte();
		MyI2C_SendACK(0);                  //发送应答，继续接收数据
	}
	DataArray[i] = MyI2C_ReceiveByte();
	MyI2C_SendACK(1);                      //发送非应答，停止接收数据
	MyI2C_Stop();
}



/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();                                //I2C初始化，用于和MPU6050通信
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  //配置电源管理寄存器1，不复位，不睡眠，不循环，使能温感，选择001X轴2的陀螺仪时钟
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);  //配置电源管理寄存器2，循环唤醒不需要，6个轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);  //内置ADC采样率分频，选最大10分频，该值越小越快
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);      //配置寄存器，外部同步不需要，低通滤波给110，最平滑的滤波
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);//加速度计配置，自测失能，量程选最大11，高通滤波失能,16g
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); //陀螺仪配置，自测失能，量程选最大11
}
 


/** 
   * @brief  得到芯片ID号
   * @param  无
   * @retval ID号
   */ 
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}


/**
  * @brief  读取六轴的数据
  * @param  六个轴的数据
  * @retval 无
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, int16_t * Temp, int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	
	uint16_t DataH, DataL;
	
	//读取加速度计的X、Y、Z轴数据
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;//高八位和低八位拼接后传递给指针指向的空间
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	//读取温度
	DataH = MPU6050_ReadReg(MPU6050_TEMP_OUT_H);
	DataL = MPU6050_ReadReg(MPU6050_TEMP_OUT_L);
	*Temp = (DataH << 8) | DataL;
	
	//读取陀螺仪的X、Y、Z轴数据
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
	
	//提供一种更简洁的思路
	//使用I2C读取多个字节的时序，从基地址开始，连续读取一片的寄存器
	//一次性把加速度值、温度值、陀螺仪值都读取出来
	//要利用数组指针
	
	//经以下代码检验，该方法存在缺陷，最后一位数据的低八位读取不成功，加长数组也不行
//	uint8_t DataArray[18];
//	MPU6050_ReceiveRegArray(MPU6050_ACCEL_XOUT_H, DataArray);
//	*AccX = (DataArray[0] << 8) | DataArray[1];
//	*AccY = (DataArray[2] << 8) | DataArray[3];
//	*AccZ = (DataArray[4] << 8) | DataArray[5];
//	*GyroX = (DataArray[10] << 8) | DataArray[11];
//	*GyroY = (DataArray[12] << 8) | DataArray[13];
//	*GyroZ = (DataArray[14] << 8) | DataArray[15];
}








