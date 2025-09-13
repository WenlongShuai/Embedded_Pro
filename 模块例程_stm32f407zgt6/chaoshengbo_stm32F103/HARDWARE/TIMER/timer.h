#ifndef _TIMER_H
#define _TIMER_H
//////////////////////////////////////////////////////////////////////////////////
#include "stm32f10x.h"

/* GPIO */
#define GPIO_RCC_GPIO_PORT 		RCC_APB2Periph_GPIOA
#define GPIO_GPIO_PIN 				GPIO_Pin_0
#define GPIO_GPIO_PORT 				GPIOA


/* TIM */
#define TIM_RCC_PERIPH 				RCC_APB1Periph_TIM2
#define TIM_x 								TIM2
#define TIMx_IRQ 							TIM2_IRQn
#define TIMx_IRQHANDLER 			TIM2_IRQHandler
#define TIMx_CHANNEL_x 				TIM_Channel_1
#define TIMx_IT_CCx	 					TIM_IT_CC1
#define TIM_NVIC_PREEMPTIONPRIORITY 	2
#define TIM_NVIC_SUBPRIORITY   				0

#define TIM_OCxPolarityConfig TIM_OC1PolarityConfig
#define TIM_GetCapturex				TIM_GetCapture1


void TIMx_CHx_Cap_Init(u32 arr,u16 psc);

#endif
