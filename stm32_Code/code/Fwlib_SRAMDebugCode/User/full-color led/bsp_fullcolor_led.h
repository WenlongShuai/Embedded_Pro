#ifndef __BSP_FULLCOLOR_LED_H__
#define __BSP_FULLCOLOR_LED_H__

#include "stm32f10x.h"

#define LED_G_GPIO_PIN 	GPIO_Pin_0
#define LED_R_GPIO_PIN 	GPIO_Pin_5
#define LED_B_GPIO_PIN 	GPIO_Pin_1

#define LED_GPIO_PORT GPIOB
#define LED_GPIO_CLK 	RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO

#define PERIOD       256-1
#define PRESCALER    2000-1

void FullColor_LED_Init(void);
void setRGBColor(uint32_t rgb);

#endif /*__BSP_FULLCOLOR_LED_H__*/
