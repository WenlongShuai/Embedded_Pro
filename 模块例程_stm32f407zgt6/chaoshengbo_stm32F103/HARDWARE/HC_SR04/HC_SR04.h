#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "timer.h"
#include "SysTick.h"
#include "stm32f10x.h"

#define RCC_TRIG_PERIPH 	RCC_APB2Periph_GPIOA
#define TRIG_PORT 				GPIOA
#define TRIG_PIN 					GPIO_Pin_1


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
