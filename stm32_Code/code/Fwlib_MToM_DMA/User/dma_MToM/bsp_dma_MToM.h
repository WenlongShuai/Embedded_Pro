#ifndef __BSP_DMA_MTOM_H__
#define __BSP_DMA_MTOM_H__

#include "stm32f10x.h"
#include <stdio.h>

#define BUFFER_SIZE   32

#define DMA_CHANNEL  							DMA1_Channel1
#define DMA_CLK  									RCC_AHBPeriph_DMA1
#define DMA_TC_FLAG  							DMA1_FLAG_TC1
#define DMA_TC_IT									DMA_IT_TC
#define DMA_IRQHANDLER					  DMA1_Channel1_IRQHandler

#define DMA_PERIPHERAL_DIR  			DMA_DIR_PeripheralSRC
#define DMA_BUFFERSIZE    				BUFFER_SIZE
#define DMA_PERIPHERAL_INC 				DMA_PeripheralInc_Enable
#define DMA_PERIPHERAL_DATASIZE  	DMA_PeripheralDataSize_Word
#define DMA_MEMORY_INC 						DMA_MemoryInc_Enable
#define DMA_MEMORY_DATASIZE  			DMA_MemoryDataSize_Word
#define DMA_MODE 									DMA_Mode_Normal
#define DMA_MToM  								DMA_M2M_Enable
#define DMA_PRIORITY  						DMA_Priority_High

#define DMA_NVIC_IRQ 							DMA1_Channel1_IRQn
#define DMA_NVIC_PRIORITYGROUP 		NVIC_PriorityGroup_1


void MToM_DMA_Config();
uint8_t buffer_Compare(const uint32_t *buffer1, uint32_t *buffer2, uint16_t bufferLen);

#endif /* __BSP_DMA_MTOM_H__ */
