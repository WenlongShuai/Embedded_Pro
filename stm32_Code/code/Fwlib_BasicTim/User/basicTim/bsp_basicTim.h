#ifndef __BSP_BASCITIM_H__
#define __BSP_BASCITIM_H__

#include "stm32f10x.h"

#define BASICTIM_7

#define BASICTIMx           	TIM7
#define BASICTIM_CLK_FUNC   	RCC_APB1PeriphClockCmd

#ifdef BASICTIM_6
#define BASICTIM_CLK   				RCC_APB1Periph_TIM6
#define BASICTIM_TRQHANDLER 	TIM6_IRQHandler
#define BASICTIM_IRQ 					TIM6_IRQn
#else
#define BASICTIM_CLK   				RCC_APB1Periph_TIM7
#define BASICTIM_TRQHANDLER 	TIM7_IRQHandler
#define BASICTIM_IRQ 					TIM7_IRQn
#endif

#define BASICTIM_PRESCALER		72-1
#define BASICTIM_PERIOD       1000-1

void BasicTim_Init(void);
void BasicTim_Delay_ms(uint16_t ms);

#endif /* __BSP_BASCITIM_H__ */
