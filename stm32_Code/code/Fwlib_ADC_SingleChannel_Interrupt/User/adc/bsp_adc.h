#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "stm32f10x.h"

#define ADC_1		0
#define ADC_2		0								
#define ADC_3		1

#if ADC_1
#define ADC_x      								ADC1
#elif ADC_2
#define ADC_x      								ADC2
#elif ADC_3
#define ADC_x      								ADC3
#endif

#define ADCx_CLK_FUNC							RCC_APB2PeriphClockCmd
#define ADCx_GPIO_CLK_FUNC				RCC_APB2PeriphClockCmd
#define ADCx_GPIO_CLK							RCC_APB2Periph_GPIOC

#if ADC_1
#define ADCx_CLK       						RCC_APB2Periph_ADC1
#elif ADC_2
#define ADCx_CLK       						RCC_APB2Periph_ADC2
#elif ADC_3
#define ADCx_CLK       						RCC_APB2Periph_ADC3
#endif

#define ADCx_CHANNEL_NUM          1
#define ADCx_IT_FLAG						  ADC_IT_EOC
#define ADCx_SAMPLETIME					  ADC_SampleTime_55Cycles5

#if ADC_1 || ADC_2
#define ADCx_IRQHANDLER  					ADC1_2_IRQHandler
#define ADCx_NVIC_IRQCHANNEL			ADC1_2_IRQn
#elif ADC_3
#define ADCx_IRQHANDLER  					ADC3_IRQHandler
#define ADCx_NVIC_IRQCHANNEL			ADC3_IRQn
#endif

#define ADCx_CHANNEL10						ADC_Channel_10			
#define ADCx_CHANNEL10_GPIO_PORT  GPIOC
#define ADCx_CHANNEL10_GPIO_PIN		GPIO_Pin_0

#define ADCx_CHANNEL11						ADC_Channel_11	
#define ADCx_CHANNEL11_GPIO_PORT  GPIOC
#define ADCx_CHANNEL11_GPIO_PIN   GPIO_Pin_1

#define ADCx_CHANNEL12						ADC_Channel_12
#define ADCx_CHANNEL12_GPIO_PORT  GPIOC
#define ADCx_CHANNEL12_GPIO_PIN   GPIO_Pin_2

#define ADCx_CHANNEL13						ADC_Channel_13
#define ADCx_CHANNEL13_GPIO_PORT  GPIOC
#define ADCx_CHANNEL13_GPIO_PIN   GPIO_Pin_3

#if ADC_1 || ADC_2
#define ADCx_CHANNEL14						ADC_Channel_14
#define ADCx_CHANNEL14_GPIO_PORT  GPIOC
#define ADCx_CHANNEL14_GPIO_PIN   GPIO_Pin_4

#define ADCx_CHANNEL15						ADC_Channel_15	
#define ADCx_CHANNEL15_GPIO_PORT  GPIOC
#define ADCx_CHANNEL15_GPIO_PIN   GPIO_Pin_5

#endif

void ADCx_Init(void);


#endif /* __BSP_ADC_H__ */
