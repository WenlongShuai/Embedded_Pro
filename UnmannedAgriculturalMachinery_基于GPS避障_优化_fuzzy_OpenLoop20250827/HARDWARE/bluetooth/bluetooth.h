#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "sys.h"

#define BLUETOOTH_USARTx 					USART3
#define BLUETOOTH_USART_RCC				RCC_APB1Periph_USART3
#define BLUETOOTH_USART_RCC_GPIO	RCC_AHB1Periph_GPIOB
				
#define BLUETOOTH_USART_RX_GPIO 	GPIOB
#define BLUETOOTH_USART_RX_PIN		GPIO_Pin_11
#define BLUETOOTH_RX_PinSourcex 	GPIO_PinSource11
#define BLUETOOTH_RX_AF 					GPIO_AF_USART3
				
#define BLUETOOTH_USART_TX_GPIO		GPIOB
#define BLUETOOTH_USART_TX_PIN		GPIO_Pin_10
#define BLUETOOTH_TX_PinSourcex 	GPIO_PinSource10
#define BLUETOOTH_TX_AF 					GPIO_AF_USART3

#define BLUETOOTH_STATE_RCC_GPIO	RCC_AHB1Periph_GPIOG
#define BLUETOOTH_STATE_GPIO			GPIOG
#define BLUETOOTH_STATE_PIN 			GPIO_Pin_1

#define BLUETOOTH_USART_IRQn			USART3_IRQn

#define BLUETOOTH_USART_REC_LEN  			256  	//定义最大接收字节数 256


#define GET_BLUETOOTH_STATE_LEVEL()   GPIO_ReadInputDataBit(BLUETOOTH_STATE_GPIO, BLUETOOTH_STATE_PIN)  //获取蓝牙模块STATE引脚电平

void Bluetooth_USART_GPIO_Init(uint32_t bound);
void Bluetooth_USART_Send_Str(uint8_t *str);

uint8_t set_Bluetooth_Param(uint8_t type, uint8_t *paramStr);




#endif /* __BLUETOOTH_H */
