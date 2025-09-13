#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

/* GPIO */
#define GPIO_RCC_GPIO_PORT 		RCC_AHB1Periph_GPIOA
#define GPIO_GPIO_PIN 				GPIO_Pin_1
#define GPIO_GPIO_PORT 				GPIOA

/* TIM */
#define TIM_RCC_PERIPH 				RCC_APB1Periph_TIM2
#define TIMx 									TIM2
#define TIMx_IRQ 							TIM2_IRQn
#define TIM_NVIC_PREEMPTIONPRIORITY 	1
#define TIM_NVIC_SUBPRIORITY   				1

/* EXTI */
#define EXTI_PortSourceGPIOx	EXTI_PortSourceGPIOA
#define EXTI_PinSourcex  			EXTI_PinSource1
#define EXTI_NVIC_PREEMPTIONPRIORITY 	2
#define EXTI_NVIC_SUBPRIORITY   			2
#define EXTI_Linex 						EXTI_Line1
#define EXTIx_IRQ 						EXTI1_IRQn

void TIMx_Init(u32 arr,u32 psc);

#endif
