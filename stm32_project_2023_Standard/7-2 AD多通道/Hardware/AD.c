#include "stm32f10x.h"                  // Device header

/**
  * @brief  ADC初始化
  * @brief  注意，在不使用DMA前提下，达到对多通道进行转换的方法是：利用单次转换，非扫描模式，不断改变该模式唯一的序列对应的通道
  * @param  无
  * @retval 无
  */
void AD_Init(void)
{
	//1、使能时钟，ADC时钟分频
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12HZ，最高14HZ
	
	
	//2、配置PA0，接收电压，原来的接收高低电平功能失效
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//第三步移到读值函数里了
	//3、配置规则组
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);
	//外设1，通道0（存在对应引脚），序列第1， 41.5个采样时间
	//如需配置扫描模式多通道，则直接复制该句修改参数即可
	//采样时间短，则转换快；采样时间长，则减少毛刺干扰
	
	
	//4、配置AD转换器：单次转换，非扫描
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 //工作模式：独立/双ADC，除了第一个参数，其他都是双ADC模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //右对齐：16位寄存器，实际数据12位，右对齐方便准确读数，左对齐放大数据，减小精度，可用于估读
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发START源选择，这里使用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                //连续转换/单次转换（是否需要手动触发）
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                      //扫描/非扫描（是否多通道）
	ADC_InitStructure.ADC_NbrOfChannel = 1;                            //扫描模式下，会用到的通道数目,非扫描，该句无效，始终为1
	
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//5、看门狗和中断配置，看个人需求，这里不用
	
	//6、开启总开关
	ADC_Cmd(ADC1, ENABLE);
	
	//7、校准
	ADC_ResetCalibration(ADC1);                       //复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//等待复位校准完成：开始复位置1，结束置0
	ADC_StartCalibration(ADC1);                       //开始校准
	while(ADC_GetCalibrationStatus(ADC1));            //等待校准完成
	
	//当选择了连续转换时，只需在在这里加一句触发转换代码就可以，接下来就无需触发了
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);               //软件触发转换
}


/**
  * @brief  得到转换后的值
  * @param  结果，0~4095
  * @retval 无
  */
uint16_t ADC_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_41Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);               //软件触发转换
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);//等待转换完成，EOC置1
	return ADC_GetConversionValue(ADC1);                  //读取转换值
}
