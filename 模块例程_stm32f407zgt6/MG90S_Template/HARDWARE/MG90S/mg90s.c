#include "mg90s.h"
#include "delay.h"

static void TIM_MG90S_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化定时器2 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM3输出比较PWM波参数
	TIM_OCInitStructure.TIM_OCIdleState = 0;  //仅用于TIM1、TIM8
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNIdleState = 0;  //仅用于TIM1、TIM8
	TIM_OCInitStructure.TIM_OCNPolarity = 0;  //仅用于TIM1、TIM8
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputNState = 0;	//仅用于TIM1、TIM8
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0x00;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);	
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
  TIM_Cmd(TIM3, ENABLE ); 	//使能定时器3
}


void MG90S_Init(void)
{
	TIM_MG90S_Init(20000-1, 72-1); //20ms
}

/*
PWM信号与180度舵机的关系：
0.5ms----------------0度；
1ms -----------------45度；
1.5ms----------------90度；
2ms -----------------135度；
2.5ms ---------------180度；
PWM信号与360舵机转速的关系：
0.5ms----------------正向最大转速；
1.5ms----------------速度为0；
2.5ms----------------反向最大转速；
*/
void MG90S_SetAngle(float Angle)
{
	//当设置转动角度为0时，设置定时器的预重装载值为500，因为初始化时定时器的ARR设置成的20000，20ms = 20000us,所以0.5ms=500us
//	TIM_SetCompare1(TIM3, Angle/180*2000+500);  
	TIM_SetCompare1(TIM3, 1000);
//	delay_ms(50);
//	TIM_SetCompare1(TIM3, 1500);
}


