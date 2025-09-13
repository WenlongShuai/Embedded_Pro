#include "bsp_generalTim_Output_PWM.h"

static void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIO外设的时钟
	GENERALTIM_GPIO_CLK_FUNC(GENERALTIM_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = GENERALTIM_CHANNEL1_PIN;
	GPIO_Init(GENERALTIM_CHANNEL1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GENERALTIM_CHANNEL2_PIN;
	GPIO_Init(GENERALTIM_CHANNEL2_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GENERALTIM_CHANNEL3_PIN;
	GPIO_Init(GENERALTIM_CHANNEL3_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GENERALTIM_CHANNEL4_PIN;
	GPIO_Init(GENERALTIM_CHANNEL4_PORT, &GPIO_InitStruct);
}

static void GeneralTim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	//打开基本定时器的时钟
	GENERALTIM_CLK_FUNC(GENERALTIM_CLK, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频因子，此成员用于通用和高级定时器的外部输入时钟时的输入滤波的分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = GENERALTIM_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = GENERALTIM_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;  //重复计数，只用于高级定时器TIM1、TIM8
	
	TIM_TimeBaseInit(GENERALTIMx, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;  //此成员只适用于TIM1、TIM8，通用定时器不用管
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  //此成员只适用于TIM1、TIM8，通用定时器不用管
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low;  //此成员只适用于TIM1、TIM8，通用定时器不用管
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;  //此成员只适用于TIM1、TIM8，通用定时器不用管
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	
	TIM_OCInitStruct.TIM_Pulse = GENERALTIM_CHANNEL1_PULSE;  //Channel1的占空比
	TIM_OC1Init(GENERALTIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(GENERALTIMx, TIM_OCPreload_Enable);
	
	TIM_OCInitStruct.TIM_Pulse = GENERALTIM_CHANNEL2_PULSE;  //Channel2的占空比
	TIM_OC2Init(GENERALTIMx, &TIM_OCInitStruct);
	TIM_OC2PreloadConfig(GENERALTIMx, TIM_OCPreload_Enable);
	
	TIM_OCInitStruct.TIM_Pulse = GENERALTIM_CHANNEL3_PULSE;  //Channel3的占空比
	TIM_OC3Init(GENERALTIMx, &TIM_OCInitStruct);
	TIM_OC3PreloadConfig(GENERALTIMx, TIM_OCPreload_Enable);

	TIM_OCInitStruct.TIM_Pulse = GENERALTIM_CHANNEL4_PULSE;  //Channel4的占空比
	TIM_OC4Init(GENERALTIMx, &TIM_OCInitStruct);
	TIM_OC4PreloadConfig(GENERALTIMx, TIM_OCPreload_Enable);

	TIM_Cmd(GENERALTIMx, ENABLE);	
}

void GeneralTim_Init(void)
{
	GPIO_Config();
	GeneralTim_Config();
}
