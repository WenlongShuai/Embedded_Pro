#include "pwm.h"

u16 period = 0;
u16 period_1 = 0;
u16 duty = 0;
u16 duty_1 = 0;
u8 CollectFlag = 0;
u8 CollectFlag_1 = 0;
extern u32 SystemCoreClock;


static void MotorA_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(MOTOR_PWMA_RCC|MOTOR_AIN1_RCC|MOTOR_AIN2_RCC, ENABLE);
	
	GPIO_PinAFConfig(MOTOR_PWMA_GPIO, GPIO_PinSource6, GPIO_AF_TIM4);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_PWMA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MOTOR_PWMA_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_AIN1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_AIN1_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_AIN2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_AIN2_GPIO, &GPIO_InitStruct);
}

static void MotorB_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(MOTOR_PWMB_RCC|MOTOR_BIN1_RCC|MOTOR_BIN2_RCC, ENABLE);
	
	GPIO_PinAFConfig(MOTOR_PWMB_GPIO, GPIO_PinSource5, GPIO_AF_TIM2);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_PWMB_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(MOTOR_PWMB_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_BIN1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_BIN1_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_BIN2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_BIN2_GPIO, &GPIO_InitStruct);
}


static void MotorA_TIMx_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
  TIM_OCInitTypeDef  TIM_OCInitStruct;

  RCC_APB1PeriphClockCmd(MOTOR_PWMA_TIMx_RCC, ENABLE);

	//配置定时器---PWMA
	TIM_TimeBaseStruct.TIM_Period = arr;
	TIM_TimeBaseStruct.TIM_Prescaler = psc;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MOTOR_PWMA_TIMx, &TIM_TimeBaseStruct);
	
	//配置定时器得PWM模式---PWMA
  TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(MOTOR_PWMA_TIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(MOTOR_PWMA_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(MOTOR_PWMA_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_PWMA_TIMx, ENABLE);
	
  TIM_Cmd(MOTOR_PWMA_TIMx, ENABLE);
}

static void MotorB_TIMx_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
  TIM_OCInitTypeDef  TIM_OCInitStruct;

  RCC_APB1PeriphClockCmd(MOTOR_PWMB_TIMx_RCC, ENABLE);

	//配置定时器---PWMB
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
	TIM_TimeBaseStruct.TIM_Period = arr;
	TIM_TimeBaseStruct.TIM_Prescaler = psc;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MOTOR_PWMB_TIMx, &TIM_TimeBaseStruct);
	
	//配置定时器得PWM模式---PWMB
  TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(MOTOR_PWMB_TIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(MOTOR_PWMB_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(MOTOR_PWMB_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_PWMB_TIMx, ENABLE);
	
  TIM_Cmd(MOTOR_PWMB_TIMx, ENABLE);
}


void Motor_Ctrl_Init(void)
{
	MotorA_GPIO_Init();
	MotorB_GPIO_Init();

	MotorA_TIMx_Init(8400-1, 0);   //84MHz / 8400 / 1 = 100us
	MotorB_TIMx_Init(8400-1, 0);  //84MHz / 8400 / 1 = 100us
}



void Remote_Ctrl(int mode)
{
	switch(mode)
	{
		case 1:  //正转
			GPIO_SetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 高电平
			GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 低电平
			break;
		case 2:  //反转
			GPIO_SetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 高电平
			GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 低电平
			break;
	}
}



