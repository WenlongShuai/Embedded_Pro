#include "bsp_fullcolor_led.h"

static void LED_GPIO_Config(void)
{
	//初始化GPIO(输入输出模式、速度)，配置CRL寄存器
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIO的时钟(GPIOB)和复用功能IO时钟(AFIO)
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	
	//TIM3的重映像(部分映像PB5)
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED_R_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED_B_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

static void TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;  //基本定时初始化
	TIM_OCInitTypeDef TIM_OCInitStruct;   //输出比较器初始化
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = PRESCALER;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);
}

void FullColor_LED_Init(void)
{
	LED_GPIO_Config();
	TIM3_Config();
}


void setRGBColor(uint32_t rgb)
{
	TIM3->CCR2 = (uint8_t)((rgb & 0x00FF0000) >> 16);
	TIM3->CCR3 = (uint8_t)((rgb & 0x0000FF00) >> 8);
	TIM3->CCR4 = (uint8_t)(rgb & 0x000000FF);
}
