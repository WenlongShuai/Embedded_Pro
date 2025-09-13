#include "bsp_advancedTim_InputCapture_MeaPWM.h"
ICxCountValueStruct ICxCountValue = {0};
static void NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = ADVANCEDTIM_CCx_IRQ;
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
	ADVANCEDTIM_GPIO_CLK_FUNC(ADVANCEDTIM_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = ADVANCEDTIM_CHANNEL1_PIN;
	GPIO_Init(ADVANCEDTIM_CHANNEL1_PORT, &GPIO_InitStruct);
}

static void AdvancedTim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	
	//打开基本定时器的时钟
	ADVANCEDTIM_CLK_FUNC(ADVANCEDTIM_CLK, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频因子，此成员用于通用和高级定时器的外部输入时钟时的输入滤波的分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = ADVANCEDTIM_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = ADVANCEDTIM_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;  //重复计数，只用于高级定时器TIM1、TIM8
	TIM_TimeBaseInit(ADVANCEDTIMx, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel = ADVANCEDTIM_CHANNELx;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	//TIM_ICInit(ADVANCEDTIMx, &TIM_ICInitStruct);
	
	//根据TIM_ICInitStruct中指定的参数配置TIM外设，以测量外部PWM信号。同时配置CC1和CC2
	TIM_PWMIConfig(ADVANCEDTIMx, &TIM_ICInitStruct);
	
	TIM_SelectInputTrigger(ADVANCEDTIMx, TIM_TS_TI1FP1);  //选择有效的触发输入信号
	TIM_SelectSlaveMode(ADVANCEDTIMx, TIM_SlaveMode_Reset);  //配置从模式控制器为复位模式
	TIM_SelectMasterSlaveMode(ADVANCEDTIMx, TIM_MasterSlaveMode_Enable);

	
//	TIM_CCxCmd(ADVANCEDTIMx, ADVANCEDTIM_CHANNELx, TIM_CCx_Enable);  //Channel1的CC1
//	TIM_CCxCmd(ADVANCEDTIMx, TIM_Channel_2, TIM_CCx_Enable);  //Channel2的CC2
	
	
	TIM_ClearITPendingBit(ADVANCEDTIMx, TIM_IT_CC1);
	
	TIM_ITConfig(ADVANCEDTIMx, TIM_IT_CC1, ENABLE);
	
	TIM_Cmd(ADVANCEDTIMx, ENABLE);	
}

void AdvancedTim_Init(void)
{
	NVIC_Config();
	GPIO_Config();
	AdvancedTim_Config();
}
