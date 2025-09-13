#ifndef __PID_USART_H
#define __PID_USART_H

#include "sys.h"

//串口接收缓冲数组大小
#define UART_RX_BUFFER_SIZE 256 

extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t receive_cmd;

void PID_USART_Init(void);
void uart_FlushRxBuffer(void);
void Usart_SendByte(uint8_t str);
void Usart_SendString(uint8_t *str, uint32_t len);

#endif /* __USARTX_H */
