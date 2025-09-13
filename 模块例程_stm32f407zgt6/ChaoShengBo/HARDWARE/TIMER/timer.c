#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////

uint8_t flag = 0;				//用于记录中断信号是上升沿还是下降沿
uint32_t number = 0;			//记录定时器中断的次数
uint32_t times = 0;			//记录回响信号的持续时间
float distance = 0;    //计算后的距离

//arr：自动重装值
//psc：时钟预分频数
void TIMx_Init(u32 arr,u32 psc)
{		
	/* 配置GPIO */ 	
	//初始化GPIO口,Trig使用推挽输出,Echo使用浮空输入
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(GPIO_RCC_GPIO_PORT,ENABLE);        //使能GPIOA的外设时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                        //选择浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_GPIO_PIN;                                    //选择GPIO_Pin_1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIO_GPIO_PORT,&GPIO_InitStructure);	

	/* 配置定时器 */
	RCC_APB1PeriphClockCmd(TIM_RCC_PERIPH,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;                        //使用时钟分频1
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;                    //向上计数
	//84MHz/84/100=10000,每秒定时器计数10000个,因此每个计数为100us
	TIM_TimeBaseInitStructure.TIM_Period = arr;                                       //自动重装器
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                                     //预分频系数
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                               //该参数仅给高级定时器使用
	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);                    //使能中断输出信号
	TIM_ClearITPendingBit(TIMx,TIM_IT_Update);

	//NVIC分配定时器的中断优先级
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;                             //指定Tim2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM_NVIC_PREEMPTIONPRIORITY;                   //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM_NVIC_SUBPRIORITY;                          //响应优先级
	NVIC_Init(&NVIC_InitStructure);

	/* 配置外部中断 */
	//AFIO映射中断引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOx, EXTI_PinSourcex);

	//EXTI中断配置
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Linex;                                  //echo使用的端口1,因此选择1号中断线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;              //上升沿和下降沿都触发中断
	EXTI_Init(&EXTI_InitStructure);

	//NVIC分配外部中断的中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             //指定中断分组
	NVIC_InitTypeDef NVIC_InitStructure2;
	NVIC_InitStructure2.NVIC_IRQChannel = EXTIx_IRQ;                          //使用的端口1,因此选择这个参数
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = EXTI_NVIC_PREEMPTIONPRIORITY;                   //抢占优先级
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = EXTI_NVIC_SUBPRIORITY;                          //响应优先级 
	NVIC_Init(&NVIC_InitStructure2);
}

