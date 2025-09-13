#ifndef __USARTX_H_
#define __USARTX_H_
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

#define BLUETOOTH_USART_IRQn			USART3_IRQn

#define BLUETOOTH_USART_REC_LEN  			256  	//定义最大接收字节数 256

extern u8  Bluetooth_USART_RX_BUF[BLUETOOTH_USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//接收状态
extern u8 Bluetooth_USART_RX_STA_LINE;    //一行数据标志	
extern u8 Bluetooth_USART_RX_STA_FRAM;   //一帧数据标志
extern u16 Bluetooth_USART_RX_STA_COUNT;

void Bluetooth_USART_GPIO_Init(uint32_t bound);
void Bluetooth_USART_Send_Str(uint8_t *str);

#endif /* __USARTX_H_ */
