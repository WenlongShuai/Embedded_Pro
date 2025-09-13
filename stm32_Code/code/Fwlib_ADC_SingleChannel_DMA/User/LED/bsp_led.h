#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

#define LED_G_GPIO_PIN 	GPIO_Pin_0
#define LED_R_GPIO_PIN 	GPIO_Pin_5
#define LED_B_GPIO_PIN 	GPIO_Pin_1

#define LED_GPIO_PORT GPIOB
#define LED_GPIO_CLK 	RCC_APB2Periph_GPIOB


#define ON    1
#define OFF   0


// '\'续行符，在续行符后面不能跟任何字符，只能有回车符'\n'
#define LED_G(a)  if(a) \
										GPIO_ResetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);\
									else \
										GPIO_SetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);
#define LED_R(a)  if(a) \
										GPIO_ResetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);\
									else \
										GPIO_SetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);
#define LED_B(a)  if(a) \
										GPIO_ResetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);\
									else \
										GPIO_SetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);
									
									
#define LED_R_TOGGLE   {LED_GPIO_PORT->ODR ^= LED_R_GPIO_PIN;}

#define LED_BGR_OFF GPIO_SetBits(LED_GPIO_PORT, LED_G_GPIO_PIN | LED_B_GPIO_PIN | LED_R_GPIO_PIN);

void LED_GPIO_Config();

#endif /*__BSP_LED_H__*/
