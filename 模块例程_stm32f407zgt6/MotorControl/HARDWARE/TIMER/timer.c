#include "timer.h"


extern int Moto;//电机PWM变量 应是Motor的
extern int Position,Target_Position; //编码器的脉冲计数
extern int Encoder,Target_Velocity;

/**************************************************************************
函数功能：定时中断初始化
入口参数：arr：自动重装值  psc：时钟预分频数 
返回  值：无
**************************************************************************/
void Timerx_Init(u16 arr,u16 psc)  
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr;               //重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;            //预分频系数
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;           //时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);               //使能定时器中断
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;        //使能按键所在的外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //使能外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;        //响应优先级3
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM1,ENABLE);
}  

/**************************************************************************
函数功能：所有的控制代码都在这里面
          TIM1控制的定时中断 
**************************************************************************/
void TIM1_UP_TIM10_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM1,TIM_FLAG_Update)==SET)//5ms定时中断
	{   
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                             //===清除定时器1中断标志位	  
		//Position = Read_Position();                      //===更新位置信息
		Encoder = Read_Velocity();												//===更新速度信息
		Key();                                          //===按键控制目标值
		//Moto = Position_PID(Position, Target_Position);    //===位置PID控制器
		Moto = Incremental_PI(Encoder, Target_Velocity);    //===位置PID控制器
		Xianfu_Pwm();                                  //===PWM限幅
		Set_Pwm(Moto);
	}
}

