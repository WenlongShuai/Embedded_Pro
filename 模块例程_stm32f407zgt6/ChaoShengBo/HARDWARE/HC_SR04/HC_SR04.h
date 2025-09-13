#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "delay.h"
#include "timer.h"
#include "stm32f4xx_conf.h"

#define RCC_TRIG_PERIPH 	RCC_AHB1Periph_GPIOA
#define TRIG_PORT 				GPIOA
#define TRIG_PIN 					GPIO_Pin_2


static void Hcsr04_TRIG_GPIO_Init(void);

/**
 * @description: 超声波模块的输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init(void);

/**
 * @description: HC-SR04触发
 * @param {*}
 * @return {*}
 */
void Hcsr04Start(void);

/**
 * @description: 读取距离 
 * @param {*}
 * @return {*}
 */
float Hcsr04Read(void);


#endif
