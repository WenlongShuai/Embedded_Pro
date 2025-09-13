#ifndef __BSP_ADVANCEDTIM_H__
#define __BSP_ADVANCEDTIM_H__

#include "stm32f10x.h"

/* TIM1¡¢TIM8 */
#define ADVANCEDTIM_8

#define ADVANCEDTIM_CLK_FUNC   		RCC_APB2PeriphClockCmd

#ifdef ADVANCEDTIM_1
#define ADVANCEDTIMx           		TIM1
#define ADVANCEDTIM_CLK   				RCC_APB2Periph_TIM1
#define ADVANCEDTIM_TRQHANDLER 		TIM1_UP_IRQHandler
#define ADVANCEDTIM_IRQ 					TIM1_UP_IRQn

#else
#define ADVANCEDTIMx           		TIM8
#define ADVANCEDTIM_CLK   				RCC_APB2Periph_TIM8
#define ADVANCEDTIM_TRQHANDLER 		TIM8_UP_IRQHandler
#define ADVANCEDTIM_IRQ 					TIM8_UP_IRQn

#endif  

#define ADVANCEDTIM_PRESCALER			72-1
#define ADVANCEDTIM_PERIOD       	1000-1

void AdvancedTim_Init(void);
void AdvancedTim_Delay_ms(uint16_t ms);

#endif /* __BSP_ADVANCEDTIM_H__ */
