#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include "motor_PID.h"
#include "fun.h"

/**
@addtogroup motor
@brief      电机驱动模块
@{
*/

#define MOTOR_PWM_TIMx	TIM1
#define MOTOR_PWM_TIMx_RCC  RCC_APB2Periph_TIM1

#define MOTOR_PWMA_RCC  RCC_AHB1Periph_GPIOE
#define MOTOR_PWMA_GPIO	GPIOE
#define MOTOR_PWMA_PIN	GPIO_Pin_9

#define MOTOR_AIN1_RCC	RCC_AHB1Periph_GPIOB
#define MOTOR_AIN1_GPIO GPIOE
#define MOTOR_AIN1_PIN  GPIO_Pin_7

#define MOTOR_AIN2_RCC	RCC_AHB1Periph_GPIOB
#define MOTOR_AIN2_GPIO GPIOE
#define MOTOR_AIN2_PIN  GPIO_Pin_8

#define MOTOR_PWMB_RCC	RCC_AHB1Periph_GPIOE
#define MOTOR_PWMB_GPIO	GPIOE
#define MOTOR_PWMB_PIN	GPIO_Pin_11

#define MOTOR_BIN1_RCC	RCC_AHB1Periph_GPIOC
#define MOTOR_BIN1_GPIO	GPIOC
#define MOTOR_BIN1_PIN	GPIO_Pin_7

#define MOTOR_BIN2_RCC	RCC_AHB1Periph_GPIOC
#define MOTOR_BIN2_GPIO	GPIOC
#define MOTOR_BIN2_PIN	GPIO_Pin_8


#define MOTOR_LEFT 	((uint16_t)0x0001) // 代表右边的电机
#define MOTOR_RIGHT ((uint16_t)0x0002) // 代表左边的电机
#define POINT_MAX_LEFT 	750             // 电机全速运行时在采样时间内100ms能够达到的最大脉冲数
#define POINT_MAX_RIGHT 730             // 电机全速运行时在采样时间内100ms能够达到的最大脉冲数

#define MAX_SPEED 5.1               // 电机能够达到的最高速度，单位为dm/s

typedef enum { MOTOR_DIRECTION_FORWARD = 0,
               MOTOR_DIRECTION_STOP,
               MOTOR_DIRECTION_BACKWARD,
               MOTOR_DIRECTION_KEEP } MOTOR_DIRECTION; // 电机的转动控制


void Motor_Config(void);
void Motor_Set_Speed(float speed0, float speed1);
void Motor_Set_Direction(MOTOR_DIRECTION dir, uint16_t motor);
MOTOR_DIRECTION Motor_Get_Direction(uint16_t motor);
void getMotorSpeed(void);

#endif /* __MOTOR_H */

