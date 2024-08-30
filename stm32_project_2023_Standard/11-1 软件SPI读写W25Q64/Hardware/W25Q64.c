#include "stm32f10x.h"                  // Device header
#include "MySPI.h"                      
#include "W25Q64_Ins.h"                  


//W25Q64初始化
void W25Q64_Init(void)
{
	MySPI_Init();
}


//读取ID号
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)//用指针方式返回两个数据
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);           //该指令用于读取ID号
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);  //抛砖引玉，发出去的数据无意义，只是为了接收数据
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;                                //上一次读到的数据放在了低八位，需要左移到高八位
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE); //将这次读到的数据放到低八位，高八位不变
	MySPI_Stop();
}


//写使能，允许存入数据
void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}


//等待忙状态解除
void W25Q64_WaitBusy(void)
{
	uint32_t TimeOut = 100000;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);           //发送读状态寄存器1指令
	while((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)//忙状态等待
	{
		TimeOut --;
		if(TimeOut == 0)
		{
			break; 
		}
	}
	MySPI_Stop();
}

//页编程，写入数据
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)//三个字节地址，要存储的数据， 数据的个数(最多256个)
{
	uint16_t i;
	W25Q64_WriteEnable();         //改变W25Q64数据的操作时序都要写使能，即存储和擦除，且时序结束后自动写失能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address >> 16);//取地址的高八位
	MySPI_SwapByte(Address >> 8); //取地址的中八位，高八位超出范围自动舍弃
	MySPI_SwapByte(Address);      //取地址的低八位
	
	for(i = 0; i < Count; i ++)
	{
		MySPI_SwapByte(DataArray[i]);
	}
	MySPI_Stop();
	
	W25Q64_WaitBusy();            //事后等待
}


//扇区擦除，全部置1
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();         //写使能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address >> 16);//取地址的高八位
	MySPI_SwapByte(Address >> 8); //取地址的中八位，高八位超出范围自动舍弃
	MySPI_SwapByte(Address);      //取地址的低八位
	MySPI_Stop();
	
	W25Q64_WaitBusy();            //事后等待
}


//读取W25Q64存放的数据
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)//三个字节地址，读取到的数据所存放的数组，数据的个数（无限制）
{
	uint32_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address >> 16);//取地址的高八位
	MySPI_SwapByte(Address >> 8); //取地址的中八位，高八位超出范围自动舍弃
	MySPI_SwapByte(Address);      //取地址的低八位
	
	for(i = 0; i < Count; i ++)
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();
}
