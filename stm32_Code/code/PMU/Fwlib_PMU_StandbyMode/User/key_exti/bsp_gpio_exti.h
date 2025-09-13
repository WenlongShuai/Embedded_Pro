#ifndef __BSP_GPIO_EXTI_H__
#define __BSP_GPIO_EXTI_H__

#include "stm32f10x.h"

#define KEY1_GPIO_CLK 								 			 RCC_APB2Periph_GPIOA
#define KEY1_GPIO_PORT_EXTI  								 GPIOA
#define KEY1_GPIO_PIN_EXTI   								 GPIO_Pin_0
#define KEY1_NVIC_IRQCHANNEL        				 EXTI0_IRQn
#define KEY1_EXTI_LINE   										 EXTI_Line0
#define KEY1_EXTI_LINE_GPIO_PORT  					 GPIO_PortSourceGPIOA
#define KEY1_EXTI_LINE_GPIO_PIN   					 GPIO_PinSource0
#define KEY1_NVIC_IRQCHANNEL_PREPRIORITY 	 	 0
#define KEY1_NVIC_IRQCHANNEL_SUBPRIORITY     1
#define KEY1_NVIC_PRIORITY_GROUP   					 NVIC_PriorityGroup_0
#define KEY1_EXTI_MODE  								  	 EXTI_Mode_Interrupt
#define KEY1_EXTI_TRIGGER   								 EXTI_Trigger_Rising

#define KEY2_GPIO_CLK 								 			 RCC_APB2Periph_GPIOC
#define KEY2_GPIO_PORT_EXTI  								 GPIOC
#define KEY2_GPIO_PIN_EXTI   								 GPIO_Pin_13
#define KEY2_NVIC_IRQCHANNEL        				 EXTI15_10_IRQn
#define KEY2_EXTI_LINE   										 EXTI_Line13
#define KEY2_EXTI_LINE_GPIO_PORT  				   GPIO_PortSourceGPIOC
#define KEY2_EXTI_LINE_GPIO_PIN   					 GPIO_PinSource13
#define KEY2_NVIC_IRQCHANNEL_PREPRIORITY 	 	 0
#define KEY2_NVIC_IRQCHANNEL_SUBPRIORITY     2
#define KEY2_NVIC_PRIORITY_GROUP   					 NVIC_PriorityGroup_0
#define KEY2_EXTI_MODE  								  	 EXTI_Mode_Interrupt
#define KEY2_EXTI_TRIGGER   								 EXTI_Trigger_Falling


void KEY_EXTI_Config();
uint8_t Key2_LongPress(void);

#endif /*__BSP_GPIO_EXTI_H__*/















