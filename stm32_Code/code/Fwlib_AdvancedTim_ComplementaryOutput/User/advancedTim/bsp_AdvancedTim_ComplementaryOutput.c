#include "bsp_AdvancedTim_ComplementaryOutput.h"

static void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIO外设的时钟
	ADVANCEDTIM_GPIO_CLK_FUNC(ADVANCEDTIM_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = ADVANCEDTIM_CHANNEL1_PIN;
	GPIO_Init(ADVANCEDTIM_CHANNEL1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADVANCEDTIM_CHANNEL1_N_PIN;
	GPIO_Init(ADVANCEDTIM_CHANNEL1_N_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ADVANCEDTIM_BKIN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ADVANCEDTIM_BKIN_PORT, &GPIO_InitStruct);
	
	// BKIN引脚默认先输出低电平
	GPIO_ResetBits(ADVANCEDTIM_BKIN_PORT,ADVANCEDTIM_BKIN_PIN);	
}

static void AdvancedTim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	
	//打开基本定时器的时钟
	ADVANCEDTIM_CLK_FUNC(ADVANCEDTIM_CLK, ENABLE);
	
	/* 定时器基本结构体 */
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //时钟分频因子，此成员用于通用和高级定时器的外部输入时钟时的输入滤波的分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = ADVANCEDTIM_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = ADVANCEDTIM_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;  //不使用重复计数
	TIM_TimeBaseInit(ADVANCEDTIMx, &TIM_TimeBaseInitStruct);
	
	/* 定时器输出比较结构体 */
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;  //经过死区后OC1的电平为高电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  //经过死区后OC1N的电平为低电平
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;  //OC1N低电平有效
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;  //OC1高电平有效
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;  //开启OC1N信号输出到对应的输出引脚
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;   //开启OC1信号输出到对应的输出引脚
	TIM_OCInitStruct.TIM_Pulse = ADVANCEDTIM_PULSE;  //占空比
	TIM_OC1Init(ADVANCEDTIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(ADVANCEDTIMx, TIM_OCPreload_Enable);
	
	/* 定时器死区和刹车结构体 */
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Enable;
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;
	// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
	// 这里配置的死区时间为152ns
	TIM_BDTRInitStruct.TIM_DeadTime = 11; //死区时间
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Enable;  //空闲模式下，定时器不工作时，OC/OCN首先输出空闲电平，然后使能输出信号=1
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Enable;  //运行模式下，定时器不工作时，OC/OCN首先输出空闲电平，然后使能输出信号=1
	TIM_BDTRConfig(ADVANCEDTIMx, &TIM_BDTRInitStruct);
	
	TIM_Cmd(ADVANCEDTIMx, ENABLE);	
	TIM_CtrlPWMOutputs(ADVANCEDTIMx, ENABLE); //主输出使能，当使用的是通用定时器时，这句不需要
}

void AdvancedTim_Init(void)
{
	GPIO_Config();
	AdvancedTim_Config();
}
