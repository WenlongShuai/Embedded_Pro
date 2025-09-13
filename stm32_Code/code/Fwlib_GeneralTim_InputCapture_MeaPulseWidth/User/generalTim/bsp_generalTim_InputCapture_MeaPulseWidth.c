#include "bsp_generalTim_InputCapture_MeaPulseWidth.h"

// 定时器输入捕获用户自定义变量结构体定义
TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0};

//配置TIM的中断优先级
static void NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = GENERALTIM_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_Init(&NVIC_InitStruct);
}

static void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIO外设的时钟
	GENERALTIM_GPIO_CLK_FUNC(GENERALTIM_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GENERALTIM_CHANNEL1_PIN;
	GPIO_Init(GENERALTIM_CHANNEL1_PORT, &GPIO_InitStruct);
}

static void GeneralTim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	
	//打开基本定时器的时钟
	GENERALTIM_CLK_FUNC(GENERALTIM_CLK, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频因子，此成员用于通用和高级定时器的外部输入时钟时的输入滤波的分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = GENERALTIM_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = GENERALTIM_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;  //重复计数，只用于高级定时器TIM1、TIM8	
	TIM_TimeBaseInit(GENERALTIMx, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel = GENERALTIM_CHANNELx;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;  //无滤波器，以fDTS采样
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;  //上升沿检测
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //无预分频器，捕获输入口上检测到的每一个边沿都触发一次捕获
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;  //通道使用直接连接方式
	
	TIM_ICInit(GENERALTIMx, &TIM_ICInitStruct);
	
	TIM_ClearITPendingBit(GENERALTIMx, GENERALTIM_CHANNEL_IT_CCx | TIM_IT_Update); //清除CC1的中断标志位和更新中断标志
	
	TIM_ITConfig(GENERALTIMx, GENERALTIM_CHANNEL_IT_CCx | TIM_IT_Update, ENABLE);  //使能中断
	
	TIM_Cmd(GENERALTIMx, ENABLE);	
}

void GeneralTim_Init(void)
{
	NVIC_Config();
	GPIO_Config();
	GeneralTim_Config();
}
