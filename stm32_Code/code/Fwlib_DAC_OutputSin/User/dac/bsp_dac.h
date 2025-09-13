#ifndef __BSP_DAC_H__
#define __BSP_DAC_H__

#include "stm32f10x.h"

/* 
DAC1 ---> PA4
DAC2 ---> PA5
*/


#define DAC_CLK_FUNC				RCC_APB1PeriphClockCmd
#define DAC_CLK			 				RCC_APB1Periph_DAC

#define DAC_GPIO_CLK_FUNC  	RCC_APB2PeriphClockCmd
#define DAC_GPIO_CLK				RCC_APB2Periph_GPIOA
#define DAC_GPIO_PORT				GPIOA
#define DAC_GPIO_PIN				GPIO_Pin_4 | GPIO_Pin_5

#define DAC_DMA_CLK_FUNC		RCC_AHBPeriphClockCmd
#define DAC_DMA_CHANNELx    DMA2_Channel3
#define DAC_DMA_CLK					RCC_AHBPeriph_DMA2


void DACx_Init(void);


#endif /* __BSP_DAC_H__ */

