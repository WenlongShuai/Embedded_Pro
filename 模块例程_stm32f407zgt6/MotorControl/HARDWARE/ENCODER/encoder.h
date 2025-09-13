#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"

#define ENCODER_TIMx TIM2
#define ENCODER_TIM_RCC RCC_APB1Periph_TIM2

#define ENCODER_TIM_GPIO_A_RCC RCC_AHB1Periph_GPIOA
#define ENCODER_PORT_A GPIOA
#define ENCODER_PIN_A GPIO_Pin_5

#define ENCODER_TIM_GPIO_B_RCC RCC_AHB1Periph_GPIOB
#define ENCODER_PORT_B GPIOB
#define ENCODER_PIN_B GPIO_Pin_3


void Encoder_Init_TIM(u16 arr,u16 psc);
int Read_Encoder_TIM(void);

int Read_Velocity(void);
int Read_Position(void);


#endif /* __ENCODER_H */
