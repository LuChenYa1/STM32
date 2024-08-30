#include "stm32f10x.h"                  // Device header

uint8_t MyDMA_size;


/**
  * @brief  DMA初始化（寄存器与寄存器）
  * @param  外设地址，寄存器地址，传输计数器的值
  * @retval 无
  */
void MyDMA_Init(uint32_t addrA, uint32_t addrB, uint8_t size)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	MyDMA_size = size;
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = addrA;                      //外设起始地址, 注意，该地址需转换整形
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据类型宽度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;        //外设地址是否自增
	DMA_InitStructure.DMA_MemoryBaseAddr = addrB;                          //寄存器起始地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        //寄存器数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                //寄存器地址是否自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     //传输方向，一般外设作为源（SRC）
	DMA_InitStructure.DMA_BufferSize = size;                               //缓存区大小，其实就是传输计数器的值，转运数据的个数
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                          //传输模式，是否自动重装,软件触发和自动重装（循环模式）只能选一个，否则陷入死循环
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;                            //选择硬件触发（pr-dr）还是软件触发（dr-dr）
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                    //软件层面的优先级，硬件层面看通道序号
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                           //选择特定通道进行初始化
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//让它暂时不启动转运
}

/**
  * @brief  启动DMA：先失能，修改传输计数器的值，再使能
  * @param  无
  * @retval 无 
  */
void MyDMA_Transfer(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_size);//设定传输计数器的值
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);//手动清除标志位，转运一次，清零一次
}
