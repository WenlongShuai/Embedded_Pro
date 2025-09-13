#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "stm32f10x.h"

#define ADC_1		1						
#define ADC_3		0

#define ADC_x      								ADC1
#define ADCx_CLK_FUNC							RCC_APB2PeriphClockCmd
#define ADCx_GPIO_CLK_FUNC				RCC_APB2PeriphClockCmd
#define ADCx_GPIO_CLK							RCC_APB2Periph_GPIOC

#define DMAx_CLK_FUNC      				RCC_AHBPeriphClockCmd
#define DMAx_CHANNELx							DMA1_Channel1

#if ADC_1
#define DMAx_CLK 									RCC_AHBPeriph_DMA1
#elif ADC_3
#define DMAx_CLK 									RCC_AHBPeriph_DMA2
#endif

#if ADC_1
#define ADCx_CLK       						RCC_APB2Periph_ADC1
#elif ADC_3
#define ADCx_CLK       						RCC_APB2Periph_ADC3
#endif

#define ADCx_CHANNEL_NUM          1
#define ADCx_SAMPLETIME					  ADC_SampleTime_55Cycles5				

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

#if ADC_1
#define ADCx_CHANNEL14						ADC_Channel_14
#define ADCx_CHANNEL14_GPIO_PORT  GPIOC
#define ADCx_CHANNEL14_GPIO_PIN   GPIO_Pin_4

#define ADCx_CHANNEL15						ADC_Channel_15	
#define ADCx_CHANNEL15_GPIO_PORT  GPIOC
#define ADCx_CHANNEL15_GPIO_PIN   GPIO_Pin_5

#endif

void ADCx_Init(void);


#endif /* __BSP_ADC_H__ */
