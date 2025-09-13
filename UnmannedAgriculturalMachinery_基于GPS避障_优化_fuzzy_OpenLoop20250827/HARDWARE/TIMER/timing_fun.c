#include "timing_fun.h"

#include "protocol.h"
#include "delay.h"


uint8_t timing_PID_flag = 0;
uint8_t timing_IMU_Flag = 0;
uint8_t timing_CSB_Flag = 0;
uint8_t timing_DIS_Flag = 0;
uint32_t timing_Period = 0;
volatile uint32_t systemTimeMs = 0;  // 系统运行时间（ms）

// 假设TIM2已经初始化，现在要修改它的预分频值
void changeTimerPrescaler(uint16_t newPeriod)
{
	uint16_t newPrescaler = 84000000 * (newPeriod / 1000.0) / 1680;
	
	timing_Period = newPeriod;
	
	// 停止定时器
	TIM5->CR1 &= ~TIM_CR1_CEN;
	
	// 修改PSC值
	TIM5->PSC = newPrescaler - 1;
	
	// 生成更新事件以重新加载预分频器值
	TIM5->EGR |= TIM_EGR_UG;
	
	// 清除更新标志
	TIM5->SR &= ~TIM_SR_UIF;
	
	// 启动定时器
	TIM5->CR1 |= TIM_CR1_CEN;
}

// 获取定时器5的周期(单位:ms)
float getTimer5Period(void)
{
	uint16_t psc = TIM5->PSC;  // 直接访问寄存器获取预分频值
	uint16_t arr = TIM5->ARR;  // 直接访问寄存器获取自动重装载值
	uint32_t timerClock;

	// 获取APB1总线时钟频率
	uint32_t apb1Clock = SystemCoreClock;

	// 根据APB1预分频器计算实际的APB1时钟频率
	uint8_t apb1Prescaler = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;
	switch(apb1Prescaler)
	{
			case 4: apb1Clock /= 2; break;
			case 5: apb1Clock /= 4; break;
			case 6: apb1Clock /= 8; break;
			case 7: apb1Clock /= 16; break;
			default: break; // 0,1,2,3 对应不分频
	}

	// 计算定时器时钟频率
	if (apb1Prescaler > 3)
	{
			timerClock = apb1Clock * 2;
	}
	else
	{
			timerClock = apb1Clock;
	}

	// 计算周期(ms)
	return (float)((psc + 1) * (arr + 1)) / timerClock * 1000.0f;
}

/**************************************************************************
函数功能：定时中断初始化
入口参数：arr：自动重装值  psc：时钟预分频数 
返回  值：无
**************************************************************************/
void Timing_Fun_Init(u16 arr,u16 psc)  
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr;               //重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;            //预分频系数
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;           //时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);               //使能定时器中断
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;        //使能按键所在的外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //使能外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7; //抢占优先级7
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //响应优先级0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM5,ENABLE);
		
	#if defined(PID_ASSISTANT_EN)
	timing_Period = getTimer5Period();     // 计算周期，单位ms
	set_computer_value(SEND_PERIOD_CMD, CURVES_CH1, &timing_Period, 1);     // 给通道 1 发送目标值
	#endif
}  


/**************************************************************************
函数功能：所有的控制代码都在这里面
          TIM1控制的定时中断 
**************************************************************************/
void TIM5_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update) == SET)//定时中断
	{   
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);                             //===清除定时器5中断标志位
//		systemTimeMs += 5;
	}
}

