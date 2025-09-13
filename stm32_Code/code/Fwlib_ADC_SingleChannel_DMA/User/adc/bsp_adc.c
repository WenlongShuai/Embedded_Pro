#include "bsp_adc.h"

uint16_t adcConversionValue = 0;

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开ADC的GPIO时钟
	ADCx_GPIO_CLK_FUNC(ADCx_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL10_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL10_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL11_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL11_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL12_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL12_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL13_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL13_GPIO_PORT, &GPIO_InitStruct);
	
	#if ADC_1
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL14_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL14_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADCx_CHANNEL15_GPIO_PIN;
	GPIO_Init(ADCx_CHANNEL15_GPIO_PORT, &GPIO_InitStruct);
	#endif
}

static void ADCx_Config(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	
	//打开ADC外设时钟
	ADCx_CLK_FUNC(ADCx_CLK, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  //一个通道不需要扫描
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_NbrOfChannel = ADCx_CHANNEL_NUM;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	
	ADC_Init(ADC_x, &ADC_InitStruct);
	
	//设置ADC外设的ADCCLK时钟(APB2的8分频)
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ADC的通道配置(采样时间、Rank(设置channel在第几个转换))
	ADC_RegularChannelConfig(ADC_x, ADCx_CHANNEL11, 1, ADCx_SAMPLETIME);
	
	//打开DMA请求
	ADC_DMACmd(ADC_x, ENABLE);
	
	//使能ADC
	ADC_Cmd(ADC_x, ENABLE);
	
	//ADC校准初始化
	ADC_ResetCalibration(ADC_x);
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	//开始ADC校准
	ADC_StartCalibration(ADC_x);
	while(ADC_GetCalibrationStatus(ADC_x));
	
	//软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

//DMA
static void DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//打开DMA外设时钟
	DMAx_CLK_FUNC(DMAx_CLK, ENABLE);
	
	DMA_InitStruct.DMA_BufferSize = 1;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&adcConversionValue;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC_x->DR);
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(DMAx_CHANNELx, &DMA_InitStruct);
	DMA_Cmd(DMAx_CHANNELx, ENABLE);
}

void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	DMA_Config();
	ADCx_Config();
}
