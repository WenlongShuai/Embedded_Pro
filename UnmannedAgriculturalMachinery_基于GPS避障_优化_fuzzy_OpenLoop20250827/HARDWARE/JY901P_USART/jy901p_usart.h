#ifndef __JY901P_USART_H
#define __JY901P_USART_H

#include "sys.h"

void JY901P_USART_Init(unsigned int uiBaud);
void JY901P_FlushRxBuffer(void);
void JY901P_SendByte(uint8_t str);
void JY901P_SendString(uint8_t *str, uint32_t len);


#endif /* __JY901P_USART_H */
