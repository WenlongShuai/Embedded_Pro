#include "pwm.h"
#include "remote.h"
#include "user.h"
u16 period = 0;
u16 period_1 = 0;
u16 duty = 0;
u16 duty_1 = 0;
u8 CollectFlag = 0;
u8 CollectFlag_1 = 0;
extern u32 SystemCoreClock;

void Motor_TIM1_GPIO_Init_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOE, ENABLE);
//    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_1);
	
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

}
void Servo_TIM2_GPIO_Init_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_1);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Motor_TIM1_PWM_Init_Out(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
    TIM_OCInitTypeDef  TIM_OCInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Period = arr;
    TIM_TimeBaseStruct.TIM_Prescaler = psc;
    
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
  
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    
    TIM_OCInitStruct.TIM_Pulse = 0;
   
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM1, &TIM_OCInitStruct);

	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}
void Servo_Remo_TIM2_PWM_Init_Out(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;


    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Period = arr;
    TIM_TimeBaseStruct.TIM_Prescaler = psc;
   
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
  
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

	TIM_OCInitTypeDef  TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
    
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    
    TIM_OCInitStruct.TIM_Pulse = 0;
    
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM2, &TIM_OCInitStruct);

    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}



void Remote_Ctrl_Init(void)
{
	Motor_TIM1_GPIO_Init_Out();
	Servo_TIM2_GPIO_Init_Out();

	Motor_TIM1_PWM_Init_Out(1000,120-1);
	Servo_Remo_TIM2_PWM_Init_Out(30000-1, 120-1);
}
void Remote_Ctrl(void){
	int out = 0;
	int out_1 = 0;
	out = 850.f*(my_abs(remote_data.forword_speed)/2000.) + 100;
	out_1 = remote_data.turn_pwm;
	if(my_abs(remote_data.forword_speed) > 2100){
		out = 0;
	}
	if(remote_data.turn_pwm < Servo_Min || remote_data.turn_pwm > Servo_Max){
		out_1 = Servo_Mid;
	}
	if(remote_data.forword_speed > 0){
		TIM_SetCompare2(TIM1, out);
		GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	}
	else if(remote_data.forword_speed < 0){
		TIM_SetCompare2(TIM1, out);
		GPIO_SetBits(GPIOE,GPIO_Pin_9);
	}
	else{
		out = 0;
		TIM_SetCompare2(TIM1, 0);
		GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	}
//	TIM_SetCompare2(TIM1, 0);
//	GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	TIM_SetCompare3(TIM2, out_1);
}

//舵机输出函数
void SERVO_SET(int value){
	int pwm;
	pwm = value + Servo_Mid;
	pwm = int16_constrain(pwm,Servo_Min,Servo_Max);//限幅

	TIM_SetCompare3(TIM2, pwm);
}



