#ifndef __BSP_CANBUS_H__
#define __BSP_CANBUS_H__

#include "stm32f10x.h"

#define CAN_x    CAN1
#define CAN_CLK_FUNC		RCC_APB1PeriphClockCmd
#define CAN_CLK					RCC_APB1Periph_CAN1

#define CAN_IRQ					USB_LP_CAN1_RX0_IRQn
#define CAN_IRQHANDLER 	USB_LP_CAN1_RX0_IRQHandler


#define CAN_GPIO_CLK_FUNC		RCC_APB2PeriphClockCmd
#define CAN_GPIO_CLK      	RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO
#define CAN_TX_GPIO_PORT   	GPIOB
#define CAN_TX_GPIO_PIN    	GPIO_Pin_9
#define CAN_RX_GPIO_PORT   	GPIOB
#define CAN_RX_GPIO_PIN    	GPIO_Pin_8

#define CAN_FILTER_PASSID    ((uint32_t)0x1234)


void CANx_Init(void);


#endif /* __BSP_CANBUS_H__ */
