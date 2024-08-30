#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[4];//存放ADC的数据寄存器转运过来的数据

/**
  * @brief  ADC初始化,连续扫描模式，循环转运
  * @brief  利用DMA对多通道进行转换
  * @param  无
  * @retval 无
  */
void AD_Init(void) 
{
	//1、使能时钟，ADC时钟分频
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK 6分频，12HZ，最高14HZ
	
	
	//2、配置PA0、PA1、PA2、PA3，接收电压，原来的接收高低电平功能失效
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	//3、配置规则组
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	//外设1，通道0（存在对应引脚），序列第1， 41.5个采样时间
	//采样时间短，则转换快；采样时间长，则减少毛刺干扰
	
	
	//4、配置AD转换器：单次转换，扫描模式
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 //工作模式：独立/双ADC，除了第一个参数，其他都是双ADC模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //右对齐：16位寄存器，实际数据12位，右对齐方便准确读数，左对齐放大数据，减小精度，可用于估读
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发START源选择，这里使用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 //连续转换/单次转换（是否需要手动触发）
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                       //扫描/非扫描（是否多通道）
	ADC_InitStructure.ADC_NbrOfChannel = 4;                            //扫描模式下，会用到的通道数目,非扫描，该句无效，始终为1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//5、看门狗和中断配置，看个人需求，这里不用
	
	//6、配置DMA，数据从外设ADC的数据寄存器转运到SRAM的数组
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;            //外设起始地址, ADC1的数据寄存器，注意，该地址需转换整形
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据类型宽度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           //外设地址不自增，始终在数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;                 //寄存器起始地址,一个数组
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        //寄存器数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                	   //寄存器地址（数组）自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     	   //传输方向，一般外设作为源（SRC）
	DMA_InitStructure.DMA_BufferSize = 4;                                      //缓存区大小，其实就是传输计数器的值，转运数据的个数
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                            //传输模式，是否自动重装,软件触发和自动重装（也叫循环模式）只能选一个，否则陷入死循环
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                               //选择硬件触发（pr-dr）还是软件触发（dr-dr）
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                      //软件层面的优先级，硬件层面看通道序号，这里只用了一个通道，优先级没有意义
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                               //选择特定转运通道进行初始化,只有DMA1的通道1能将ADC1作为触发源
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//启动转运
	//开始工作三条件：传输计数器值不为0；DMA使能；有触发信号，到这一步，只剩触发源没有选择
	ADC_DMACmd(ADC1, ENABLE);      //DMA1的通道1有三个可选触发源，必须选择一个或多个，以输出触发信号
	
	
	//7、开启总开关
	ADC_Cmd(ADC1, ENABLE);
	
	//8、校准
	ADC_ResetCalibration(ADC1);                       //复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//等待复位校准完成：开始复位置1，结束置0
	ADC_StartCalibration(ADC1);                       //开始校准
	while(ADC_GetCalibrationStatus(ADC1) == SET);     //等待校准完成
	
	//当选择了连续转换时，只需在在这里加一句触发转换代码就可以，接下来就无需触发了
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);           //软件触发转换
}
