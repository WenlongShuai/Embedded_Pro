#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// 按键引脚定义  KEY0
#define KEY0_GPIO_PORT     GPIOE
#define KEY0_GPIO_PIN      GPIO_Pin_4
#define KEY0_GPIO_CLK      RCC_AHB1Periph_GPIOE

// 按键引脚定义  KEY1
#define KEY1_GPIO_PORT     GPIOE
#define KEY1_GPIO_PIN      GPIO_Pin_3
#define KEY1_GPIO_CLK      RCC_AHB1Periph_GPIOE

void KEY_Init(void);


#endif /* __KEY_H */
