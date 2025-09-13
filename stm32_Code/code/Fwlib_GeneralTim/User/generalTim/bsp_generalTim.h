#ifndef __BSP_GENERALTIM_H__
#define __BSP_GENERALTIM_H__

#include "stm32f10x.h"

#define GENERALTIM_4

#define GENERALTIM_CLK_FUNC   	RCC_APB1PeriphClockCmd

#ifdef GENERALTIM_2
#define GENERALTIMx           	TIM2
#define GENERALTIM_CLK   				RCC_APB1Periph_TIM2
#define GENERALTIM_TRQHANDLER 	TIM2_IRQHandler
#define GENERALTIM_IRQ 					TIM2_IRQn

#elif defined(GENERALTIM_3)
#define GENERALTIMx           	TIM3
#define GENERALTIM_CLK   				RCC_APB1Periph_TIM3
#define GENERALTIM_TRQHANDLER 	TIM3_IRQHandler
#define GENERALTIM_IRQ 					TIM3_IRQn

#elif defined(GENERALTIM_4)
#define GENERALTIMx           	TIM4
#define GENERALTIM_CLK   				RCC_APB1Periph_TIM4
#define GENERALTIM_TRQHANDLER 	TIM4_IRQHandler
#define GENERALTIM_IRQ 					TIM4_IRQn

#elif defined(GENERALTIM_5)
#define GENERALTIMx           	TIM5
#define GENERALTIM_CLK   				RCC_APB1Periph_TIM5
#define GENERALTIM_TRQHANDLER 	TIM5_IRQHandler
#define GENERALTIM_IRQ 					TIM5_IRQn
#endif  

#define GENERALTIM_PRESCALER		72-1
#define GENERALTIM_PERIOD       1000-1

void GeneralTim_Init(void);
void GeneralTim_Delay_ms(uint16_t ms);

#endif /* __BSP_GENERALTIM_H__ */
