#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
 	
#define MOTOR_PWMA_TIMx	TIM4
#define MOTOR_PWMA_TIMx_RCC  RCC_APB1Periph_TIM4

#define MOTOR_PWMA_RCC  RCC_AHB1Periph_GPIOB
#define MOTOR_PWMA_GPIO	GPIOB
#define MOTOR_PWMA_PIN	GPIO_Pin_6

#define MOTOR_AIN1_RCC	RCC_AHB1Periph_GPIOB
#define MOTOR_AIN1_GPIO GPIOB
#define MOTOR_AIN1_PIN  GPIO_Pin_7

#define MOTOR_AIN2_RCC	RCC_AHB1Periph_GPIOB
#define MOTOR_AIN2_GPIO GPIOB
#define MOTOR_AIN2_PIN  GPIO_Pin_8

#define MOTOR_PWMB_TIMx	TIM2
#define MOTOR_PWMB_TIMx_RCC  RCC_APB1Periph_TIM2

#define MOTOR_PWMB_RCC	RCC_AHB1Periph_GPIOA
#define MOTOR_PWMB_GPIO	GPIOA
#define MOTOR_PWMB_PIN	GPIO_Pin_5

#define MOTOR_BIN1_RCC	RCC_AHB1Periph_GPIOA
#define MOTOR_BIN1_GPIO	GPIOA
#define MOTOR_BIN1_PIN	GPIO_Pin_4

#define MOTOR_BIN2_RCC	RCC_AHB1Periph_GPIOA
#define MOTOR_BIN2_GPIO	GPIOA
#define MOTOR_BIN2_PIN	GPIO_Pin_6


#define Remote_Max_1 2045
#define Remote_Mid_1 1545
#define Remote_Min_1 1043

#define Remote_Max_0 2038
#define Remote_Mid_0 1536
#define Remote_Min_0 1035

#define Servo_Max 1700
#define Servo_Mid 1320 //舵机中位，需自行调整确定
#define Servo_Min 900

#define MODE_UP 2020
#define MODE_MID 1518
#define MODE_DOWN 1016

void Motor_Ctrl_Init(void);
void Remote_Ctrl(int mode);




#endif
