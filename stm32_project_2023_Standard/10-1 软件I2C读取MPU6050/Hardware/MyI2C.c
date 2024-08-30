#include "stm32f10x.h"                  // Device header
#include "Delay.h"                  // Device header

//软件模拟IIC，SCL和SDA的电平变化都由软件实现
#define SCL_PORT GPIOA
#define SCL_PIN  GPIO_Pin_1
#define SDA_PORT GPIOA
#define SDA_PIN  GPIO_Pin_3


/**
  * @brief  I2C初始化
  * @param  无
  * @retval 无
  */
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = SCL_PIN | SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, SCL_PIN | SDA_PIN);//开始之前，先拉高SDA和SCL，方便拉低电平进入时序
}



/**
  * @brief  主机向SCL写入一位数据
  * @param  1/0
  * @retval 无
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(SCL_PORT, SCL_PIN, (BitAction)BitValue);
	Delay_us(10);
}


/**
  * @brief  主机向SDA写入一位数据
  * @param  1/0
  * @retval 无
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(SDA_PORT, SDA_PIN, (BitAction)BitValue);
	Delay_us(10);
}


/**
  * @brief  主机读SDA：主机的SDA已经被配置为了开漏输出模式，只有浮空状态才能接收数据
  * @brief  因此需要事先拉高电平，巧妙的是，主机读SDA之前需要转交SDA控制权给从机，释放SDA，正好将电平拉高变为浮空状态
  * @param  无
  * @retval SDA的值
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(SDA_PORT, SDA_PIN);
	Delay_us(10);
	return BitValue;
}


 
/**
  * @brief  起始条件
  * @param  无
  * @retval 无
  */
void MyI2C_Start(void)
{
	//始终保证SCL除了终止条件，所有单元都以低电平结束，该特性由软件实现 
	MyI2C_W_SDA(1);//起始条件前，SCL和SDA是高电平，但重复起始条件前，SCL是低电平，SDA不确定（应答/非应答）
	MyI2C_W_SCL(1);//如果先释放SCL，在SCL高电平期间释放SDA产生下降沿，会被判定为终止条件，故优先释放SDA
	MyI2C_W_SDA(0);//在SCL高电平期间SDA产生下降沿，触发起始条件
	MyI2C_W_SCL(0);
}


/**
  * @brief  终止条件
  * @param  无
  * @retval 无
  */
void MyI2C_Stop(void)
{
	//始终保证SCL除了终止条件，所有单元都以低电平结束，该特性由软件实现 
	MyI2C_W_SDA(0);//在终止条件之前，SCL已经经历过 0->1(应答)  ->0 ，但SDA不确定，所以只有SDA需要拉低
	MyI2C_W_SCL(1);//在SCL高电平期间SDA产生上升沿，触发终止条件
	MyI2C_W_SDA(1);
}


/**
  * @brief  主机发送一个字节
  * @param  Byte：字节
  * @retval 无
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));//特性：非0即1
		MyI2C_W_SCL(1);                 //从机读取
		MyI2C_W_SCL(0);                 //SCL恢复原状态
	}
}


/**
  * @brief  主机接收一个字节
  * @param  无
  * @retval Byte:接收的字节
  */
 uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte  = 0x00;//论局部变量赋值0的重要性之找了大半个小时的BUG
	MyI2C_W_SDA(1);         //释放SDA，主机转交控制权给从机
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);     //拉高SCL，读取SDA
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);     //SCL恢复原状态，从机向SDA发送数据位
	}
	return Byte;
}


/**
  * @brief  主机发送应答
  * @param  ACKBit：应答
  * @retval 无
  */
void MyI2C_SendACK(uint8_t ACKBit)
{
	MyI2C_W_SDA(ACKBit);//特性：非0即1
	MyI2C_W_SCL(1);     //从机读取
	MyI2C_W_SCL(0);     //SCL恢复原状态 
}


/**
  * @brief  主机接收应答
  * @param  无
  * @retval ACKBit:接收的应答
  */
uint8_t MyI2C_ReceiveACK(void)
{
	uint8_t  ACKBit = 0;
	MyI2C_W_SDA(1);//释放SDA，主机转交控制权给从机
	MyI2C_W_SCL(1);//拉高SCL，读取SDA
	if(MyI2C_R_SDA() == 1)ACKBit = 1;
	MyI2C_W_SCL(0);//SCL恢复原状态，从机向SDA发送数据位
	return ACKBit;
}


/**
  * @brief  查找总线设备
  * @param  无
  * @retval 无
  */
void MyI2C_FindPeriph(void)
{
	uint8_t Address = 0x00,  ArrayAddress[256], ACKBit = 3, i = 0;
	while(Address ++ && Address < 0x7F)           //0x7F = 0111 1111,左移后为地址最大值 1111 1110
	{
		MyI2C_Start();
		MyI2C_SendByte(Address << 1);             //左移一位，让最低位始终为0（写模式）
		ACKBit = MyI2C_ReceiveACK();
		MyI2C_Stop();
		if(ACKBit == 0)ArrayAddress[i]  = Address;//接收应答，存储该地址
		i++;
	}
}
