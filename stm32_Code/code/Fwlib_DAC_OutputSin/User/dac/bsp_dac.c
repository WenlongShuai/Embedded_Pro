#include "bsp_dac.h"

const uint16_t sinData12bit[] = {2048, 2460, 2856, 3218, 3532, 3786, 3969, 4072, 
														4093, 4031, 3887, 3668, 3382, 3042, 2661, 2255, 
														1841, 1435, 1054, 714, 428, 209, 65, 3, 
														24, 127, 310, 564, 878, 1240, 1636, 2048};

uint32_t dualSinData12bit[32] = {0};
	
static void DAC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//打开DMA时钟
	DAC_DMA_CLK_FUNC(DAC_DMA_CLK, ENABLE);
	
	DMA_InitStruct.DMA_BufferSize = sizeof(sinData12bit)/sizeof(sinData12bit[0]);
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dualSinData12bit;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DAC->DHR12RD;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_Init(DAC_DMA_CHANNELx, &DMA_InitStruct);
	DMA_Cmd(DAC_DMA_CHANNELx, ENABLE);
}



static void DAC_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeInitStruct;
	
	//打开TIM的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频因子，控制tDTS周期
	TIM_TimeInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
	TIM_TimeInitStruct.TIM_Period = (20-1);  //定时周期为20
	TIM_TimeInitStruct.TIM_Prescaler = 0x0;
	TIM_TimeInitStruct.TIM_RepetitionCounter = 0; //重复计数器，TIM1和TIM8才使用
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeInitStruct);
	
	//选择TIMx触发器输出模式，作为DAC的触发源
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
	//使能TIM
	TIM_Cmd(TIM2, ENABLE);
}

static void DAC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIO时钟
	DAC_GPIO_CLK_FUNC(DAC_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = DAC_GPIO_PIN;
	GPIO_Init(DAC_GPIO_PORT, &GPIO_InitStruct);
}

static void DAC_Config(void)
{
	DAC_InitTypeDef DAC_InitStruct;
	
	//打开DAC外设时钟
	DAC_CLK_FUNC(DAC_CLK, ENABLE);
	
	//配置DAC1
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_T2_TRGO;
	DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;  //噪声/三角波是否使能
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;  //该选项需要在DAC_WaveGeneration使能的情况下才有效
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;  //使能输出缓冲，可以减小输出阻抗
	DAC_Init(DAC_Channel_1, &DAC_InitStruct);
	//使能DAC
	DAC_Cmd(DAC_Channel_1, ENABLE);
	
	//因为使用双通道DAC，所以以同样的配置 配置DAC2
	DAC_Init(DAC_Channel_2, &DAC_InitStruct);
	//使能DAC
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	//使能DMA，使用DMA设置DAC1的数据时，同时更新DAC2的值
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void DACx_Init(void)
{
	uint8_t i = 0;
	DAC_GPIO_Config();
	DAC_TIM_Config();
	DAC_Config();
	
	//将单通道正弦波数据填充到多通道右对齐
	for(i = 0;i < 32;i++)
	{
		dualSinData12bit[i] = (sinData12bit[i] << 16) | sinData12bit[i];
	}
	
	DAC_DMA_Config();
}

