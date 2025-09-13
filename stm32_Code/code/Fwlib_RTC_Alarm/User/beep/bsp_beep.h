#ifndef __BSP_BEEP_H__
#define __BSP_BEEP_H__

#include "stm32f10x.h"

#define BEEP_GPIO_CLK_FUNC  RCC_APB2PeriphClockCmd
#define BEEP_GPIO_CLK				RCC_APB2Periph_GPIOA
#define BEEP_GPIO_PORT  		GPIOA
#define BEEP_GPIO_PIN   		GPIO_Pin_8


void BEEP_GPIO_Config(void);
void BEEP_Cmd(FunctionalState NewState);

#endif  /*__BSP_BEEP_H__*/

