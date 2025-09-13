#include "timing_fun.h"

#include "protocol.h"
#include "delay.h"


uint8_t timing_PID_flag = 0;
uint8_t timing_IMU_Flag = 0;
uint8_t timing_CSB_Flag = 0;
uint8_t timing_DIS_Flag = 0;
uint32_t timing_Period = 0;
volatile uint32_t systemTimeMs = 0;  // ϵͳ����ʱ�䣨ms��

// ����TIM2�Ѿ���ʼ��������Ҫ�޸�����Ԥ��Ƶֵ
void changeTimerPrescaler(uint16_t newPeriod)
{
	uint16_t newPrescaler = 84000000 * (newPeriod / 1000.0) / 1680;
	
	timing_Period = newPeriod;
	
	// ֹͣ��ʱ��
	TIM5->CR1 &= ~TIM_CR1_CEN;
	
	// �޸�PSCֵ
	TIM5->PSC = newPrescaler - 1;
	
	// ���ɸ����¼������¼���Ԥ��Ƶ��ֵ
	TIM5->EGR |= TIM_EGR_UG;
	
	// ������±�־
	TIM5->SR &= ~TIM_SR_UIF;
	
	// ������ʱ��
	TIM5->CR1 |= TIM_CR1_CEN;
}

// ��ȡ��ʱ��5������(��λ:ms)
float getTimer5Period(void)
{
	uint16_t psc = TIM5->PSC;  // ֱ�ӷ��ʼĴ�����ȡԤ��Ƶֵ
	uint16_t arr = TIM5->ARR;  // ֱ�ӷ��ʼĴ�����ȡ�Զ���װ��ֵ
	uint32_t timerClock;

	// ��ȡAPB1����ʱ��Ƶ��
	uint32_t apb1Clock = SystemCoreClock;

	// ����APB1Ԥ��Ƶ������ʵ�ʵ�APB1ʱ��Ƶ��
	uint8_t apb1Prescaler = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;
	switch(apb1Prescaler)
	{
			case 4: apb1Clock /= 2; break;
			case 5: apb1Clock /= 4; break;
			case 6: apb1Clock /= 8; break;
			case 7: apb1Clock /= 16; break;
			default: break; // 0,1,2,3 ��Ӧ����Ƶ
	}

	// ���㶨ʱ��ʱ��Ƶ��
	if (apb1Prescaler > 3)
	{
			timerClock = apb1Clock * 2;
	}
	else
	{
			timerClock = apb1Clock;
	}

	// ��������(ms)
	return (float)((psc + 1) * (arr + 1)) / timerClock * 1000.0f;
}

/**************************************************************************
�������ܣ���ʱ�жϳ�ʼ��
��ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
����  ֵ����
**************************************************************************/
void Timing_Fun_Init(u16 arr,u16 psc)  
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr;               //��װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;            //Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;           //ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);               //ʹ�ܶ�ʱ���ж�
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;        //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7; //��ռ���ȼ�7
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //��Ӧ���ȼ�0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM5,ENABLE);
		
	#if defined(PID_ASSISTANT_EN)
	timing_Period = getTimer5Period();     // �������ڣ���λms
	set_computer_value(SEND_PERIOD_CMD, CURVES_CH1, &timing_Period, 1);     // ��ͨ�� 1 ����Ŀ��ֵ
	#endif
}  


/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
          TIM1���ƵĶ�ʱ�ж� 
**************************************************************************/
void TIM5_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update) == SET)//��ʱ�ж�
	{   
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);                             //===�����ʱ��5�жϱ�־λ
//		systemTimeMs += 5;
	}
}

