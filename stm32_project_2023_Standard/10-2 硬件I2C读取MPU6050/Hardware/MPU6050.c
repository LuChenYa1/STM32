#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0 //(写模式)


/**
  * @brief  针对等待标志位的超时退出机制,防止循环卡死
  * @param  I2Cx:外设
  * @param  I2C_EVENT:标志位  
  * @retval 无
  */
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint16_t TimeOut = 10000;
	while(I2C_CheckEvent(I2C2, I2C_EVENT) != SUCCESS)
	{
		TimeOut --;
		if(TimeOut == 0)
		{
			break;
		}
	}
}



/**
  * @brief  向指定外设的寄存器写入数据
  * @param  RegAddress：寄存器地址
  * @param  Data：要写入的数据
  * @retval 无
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)//寄存器地址Register Address
{
	//注意：每读或写一个字节，寄存器地址指针都会后移一位，指向新的寄存器
 
	I2C_GenerateSTART(I2C2, ENABLE);                                      //发送起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);                //EV5事件标志位置一，说明起始条件已发送
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);//发送外设地址，该函数自带接收应答
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);  //EV6事件标志位置一，说明地址已发送
	
	I2C_SendData(I2C2, RegAddress);                                       //将寄存器地址写入DR寄存器中
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);          //从该循环出来后，数据正在发送中
	//当数据一次性从DR寄存器转移到移位寄存器时，DR空，移位非空，触发EV8事件(数据发送中); 参数列表没有EV8_1,不用等待该事件
	
	I2C_SendData(I2C2, Data);                                             //发送数据
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);           //从该循环出来后，数据发送完毕
	//DR寄存器和移位寄存器都空，触发EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);                                       //发送终止条件
}



/**
  * @brief  向指定外设的寄存器读取数据
  * @param  RegAddress：寄存器地址
  * @retval Data：读取的数据
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	//复合时序第一部分
	I2C_GenerateSTART(I2C2, ENABLE);                                      //发送起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);                //EV5事件标志位置一，说明起始条件已发送
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);//发送外设地址，该函数自带接收应答
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);  //EV6事件标志位置一，说明地址已发送
	
	I2C_SendData(I2C2, RegAddress);                                       //将寄存器地址写入DR寄存器中
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);           //从该循环出来后，数据发送完毕
	
	//第二部分：读
	I2C_GenerateSTART(I2C2, ENABLE);                                      //重复起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);                //等待EV5
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);   //发送从机地址，读模式，系统自动将地址最后一位置1
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);     //等待EV6
	
	
	I2C_GenerateSTOP(I2C2, ENABLE);                                       //STOP波形在接收至少一个数据以后发送
	I2C_AcknowledgeConfig(I2C2, DISABLE);                                 //ACK给非应答0
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);              //等待EV7事件，即DR有值可读取
	Data = I2C_ReceiveData(I2C2);                                                //读取DR寄存器的值
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);                                  //将ACK位恢复为1

	return Data;
	
	//补充：
	//当读取多个字节时，每次读取前都要等待EV7（RxNE = 1，即DR有值）
	//读取最后一个字节前，设置ACK = 0和STOP请求,再等待EV7_1,即可发送非应答
	//ACK是主机接收数据后发送的应答位的值，可手动修改以停止接收数据
	
//	uint8_t Num = 4;//代表接收四个字节 
//	for(uint8_t i = 0; i < Num; i++)
//	{
//		if(i == Num - 1) 
//		{
//			I2C_GenerateSTOP(I2C2, ENABLE);      
//			I2C_AcknowledgeConfig(I2C2, DISABLE);
//		}
//		while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
//	    I2C_ReceiveData(I2C2);
//	}	

}
 

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
	//I2C初始化，用于STM32和MPU6050通信
	//1、使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2、初始化GPIO， B10、B11为I2C2外设的SCL、SDA对应复用引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//3、I2C结构体初始化
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;        //不使用SMBus协议
	I2C_InitStructure.I2C_ClockSpeed =  50000;        //传输速度：标准100KHZ以下，高速400KHZ以下
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//低于标准速度时，占空比设置没有用，这里随便选
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;       //默认给应答，之后再修改
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//STM32作为从机能响应几位地址
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;         //自身作为从机的地址，随便给，地址最后一位主机会根据读写模式自动置1或0
	I2C_Init(I2C2, &I2C_InitStructure);
	
	//4、使能总开关
	I2C_Cmd(I2C2, ENABLE);
	
	//5、初始化MPU6050
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
