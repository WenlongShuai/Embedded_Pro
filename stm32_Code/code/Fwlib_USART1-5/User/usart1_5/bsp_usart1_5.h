#ifndef __BSP_USART1_5_H__
#define __BSP_USART1_5_H__

#include "stm32f10x.h"
#include <stdio.h>

#define DEBUG_USART1   1
#define DEBUG_USART2   0
#define DEBUG_USART3   0
#define DEBUG_USART4   0
#define DEBUG_USART5   0

#if 0
#if DEBUG_USART1

#define USARTx										 USART1
#define USART_TX_GPIO_PORT				 GPIOA
#define USART_TX_GPIO_PIN 				 GPIO_Pin_9
#define USART_RX_GPIO_PORT  			 GPIOA
#define USART_RX_GPIO_PIN 				 GPIO_Pin_10
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOA
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
																	 
#define USART_CLK 								 RCC_APB2Periph_USART1
#define USART_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_IRQ  					 USART1_IRQn
#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1
#define USART_IRQHANDLER           USART1_IRQHandler

#elif DEBUG_USART2

#define USARTx										 USART2
#define USART_TX_GPIO_PORT				 GPIOA
#define USART_TX_GPIO_PIN 				 GPIO_Pin_2
#define USART_RX_GPIO_PORT 				 GPIOA
#define USART_RX_GPIO_PIN 				 GPIO_Pin_3
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOA
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
																	 
#define USART_CLK 								 RCC_APB1Periph_USART2
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_IRQ  					 USART2_IRQn
#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1
#define USART_IRQHANDLER           USART2_IRQHandler


#elif DEBUG_USART3

#define USARTx										 USART3
#define USART_TX_GPIO_PORT				 GPIOB
#define USART_TX_GPIO_PIN 				 GPIO_Pin_10
#define USART_RX_GPIO_PORT	  		 GPIOB
#define USART_RX_GPIO_PIN  				 GPIO_Pin_11
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOB
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
																	 
#define USART_CLK 								 RCC_APB1Periph_USART3
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_IRQ  					 USART3_IRQn
#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1
#define USART_IRQHANDLER           USART3_IRQHandler

#elif DEBUG_USART4

#define USARTx										 UART4
#define USART_TX_GPIO_PORT				 GPIOC
#define USART_TX_GPIO_PIN  				 GPIO_Pin_10
#define USART_RX_GPIO_PORT 				 GPIOC
#define USART_RX_GPIO_PIN 				 GPIO_Pin_11
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOC
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
																	 
#define USART_CLK 								 RCC_APB1Periph_UART4
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_IRQ  					 UART4_IRQn
#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1
#define USART_IRQHANDLER           UART4_IRQHandler


#elif DEBUG_USART5

#define USARTx										 UART5
#define USART_TX_GPIO_PORT				 GPIOC
#define USART_TX_GPIO_PIN 				 GPIO_Pin_12
#define USART_RX_GPIO_PORT 				 GPIOD
#define USART_RX_GPIO_PIN 				 GPIO_Pin_2
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
																	 
#define USART_CLK 								 RCC_APB1Periph_UART5
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_IRQ  					 UART5_IRQn
#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1
#define USART_IRQHANDLER           UART5_IRQHandler

#endif

#else
#if DEBUG_USART1
#define USARTx										 USART1
#define USART_TX_GPIO_PORT				 GPIOA
#define USART_TX_GPIO_PIN 				 GPIO_Pin_9
#define USART_RX_GPIO_PORT  			 GPIOA
#define USART_RX_GPIO_PIN 				 GPIO_Pin_10
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOA
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd

#define USART_CLK 								 RCC_APB2Periph_USART1
#define USART_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd
#define USART_NVIC_IRQ  					 USART1_IRQn
#define USART_IRQHANDLER           USART1_IRQHandler

#elif DEBUG_USART2
#define USARTx										 USART2
#define USART_TX_GPIO_PORT				 GPIOA
#define USART_TX_GPIO_PIN 				 GPIO_Pin_2
#define USART_RX_GPIO_PORT 				 GPIOA
#define USART_RX_GPIO_PIN 				 GPIO_Pin_3
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOA
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd

#define USART_CLK 								 RCC_APB1Periph_USART2
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_NVIC_IRQ  					 USART2_IRQn
#define USART_IRQHANDLER           USART2_IRQHandler

#elif DEBUG_USART3
#define USARTx										 USART3
#define USART_TX_GPIO_PORT				 GPIOB
#define USART_TX_GPIO_PIN 				 GPIO_Pin_10
#define USART_RX_GPIO_PORT	  		 GPIOB
#define USART_RX_GPIO_PIN  				 GPIO_Pin_11
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOB
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd

#define USART_CLK 								 RCC_APB1Periph_USART3
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_IRQHANDLER           USART3_IRQHandler
#define USART_NVIC_IRQ  					 USART3_IRQn

#elif DEBUG_USART4
#define USARTx										 UART4
#define USART_TX_GPIO_PORT				 GPIOC
#define USART_TX_GPIO_PIN  				 GPIO_Pin_10
#define USART_RX_GPIO_PORT 				 GPIOC
#define USART_RX_GPIO_PIN 				 GPIO_Pin_11
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOC
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd

#define USART_CLK 								 RCC_APB1Periph_UART4
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_NVIC_IRQ  					 UART4_IRQn
#define USART_IRQHANDLER           UART4_IRQHandler

#elif DEBUG_USART5
#define USARTx										 UART5
#define USART_TX_GPIO_PORT				 GPIOC
#define USART_TX_GPIO_PIN 				 GPIO_Pin_12
#define USART_RX_GPIO_PORT 				 GPIOD
#define USART_RX_GPIO_PIN 				 GPIO_Pin_2
#define UASRT_GPIO_CLK 						 RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
#define USART_GPIO_RCC_PeriphClockCmd	 RCC_APB2PeriphClockCmd

#define USART_CLK 								 RCC_APB1Periph_UART5
#define USART_RCC_PeriphClockCmd	 RCC_APB1PeriphClockCmd
#define USART_NVIC_IRQ  					 UART5_IRQn
#define USART_IRQHANDLER           UART5_IRQHandler

#endif															 

#define USART_BAUDRATE 						 115200
#define USART_WORDLENGTH 					 USART_WordLength_8b
#define USART_PARITY 							 USART_Parity_No
#define USART_STOPBITS 						 USART_StopBits_1
#define USART_MODE 								 (USART_Mode_Rx | USART_Mode_Tx)
#define USART_HARDWAREFLOWCONTROL  USART_HardwareFlowControl_None

#define USART_NVIC_PREEPRIORITY 	 1
#define USART_NVIC_SUBPRIORITY 		 1
#define USART_NVIC_PRIORITYGROUP 	 NVIC_PriorityGroup_1

#endif

void USART_GPIO_Init();
void Usart_SendByteData(USART_TypeDef* USART, uint8_t sendData);
void Usart_SendHalfWordData(USART_TypeDef* USART, uint16_t sendData);
void Usart_SendArrayData(USART_TypeDef* USART, uint8_t *sendData, uint8_t arrayLen);
void Usart_SendStrData(USART_TypeDef* USART, uint8_t *sendData);


#endif /* __BSP_USART1_5_H__ */

