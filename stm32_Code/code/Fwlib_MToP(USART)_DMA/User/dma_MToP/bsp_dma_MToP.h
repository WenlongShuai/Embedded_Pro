#ifndef __BSP_DMA_MTOP_H__
#define __BSP_DMA_MTOP_H__

#include "stm32f10x.h"
#include <stdio.h>

#define BUFFER_SIZE   5000

#define DMA_CHANNEL  							DMA1_Channel4
#define DMA_CLK  									RCC_AHBPeriph_DMA1
#define DMA_TC_FLAG  							DMA1_FLAG_TC4

#define DMA_PERIPHERAL_DIR  			DMA_DIR_PeripheralDST
#define DMA_BUFFERSIZE    				BUFFER_SIZE
#define DMA_PERIPHERAL_INC 				DMA_PeripheralInc_Disable
#define DMA_PERIPHERAL_DATASIZE  	DMA_PeripheralDataSize_Byte
#define DMA_MEMORY_INC 						DMA_MemoryInc_Enable
#define DMA_MEMORY_DATASIZE  			DMA_MemoryDataSize_Byte
#define DMA_MODE 									DMA_Mode_Normal
#define DMA_MToM  								DMA_M2M_Disable
#define DMA_PRIORITY  						DMA_Priority_High

#define DEBUG_USART  							USART1
#define USART_CLK  								RCC_APB2Periph_USART1
#define USART_BAUDRATE   					115200
#define USART_HARDWAREFLOWCTRL  	USART_HardwareFlowControl_None
#define USART_MODE   							(USART_Mode_Tx | USART_Mode_Tx)
#define USART_PARITY  						USART_Parity_No
#define USART_STOPBITS 						USART_StopBits_1
#define USART_WORDLENGTH  				USART_WordLength_8b

#define USART_GPIO_CLK 						RCC_APB2Periph_GPIOA
#define USART_GPIO_TX_PORT 				GPIOA
#define USART_GPIO_TX_PIN  				GPIO_Pin_9
#define USART_GPIO_RX_PORT 				GPIOA
#define USART_GPIO_RX_PIN  				GPIO_Pin_10



void MToP_DMA_Config();
void USART_DMA_Config();
void Usart_SendByteData(USART_TypeDef* USART, uint8_t sendData);

#endif /* __BSP_DMA_MTOP_H__ */
