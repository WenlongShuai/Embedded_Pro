#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "sys.h"
#include "delay.h"

/* GPIO */
#define GPIO_RCC_GPIO_PORT 		RCC_APB2Periph_GPIOA
#define GPIO_GPIO_PIN 				GPIO_Pin_0
#define GPIO_GPIO_PORT 				GPIOA

/* TIM */
#define TIM_RCC_PERIPH 				RCC_APB1Periph_TIM2
#define TIM_x 								TIM2
#define TIMx_IRQ 							TIM2_IRQn
#define TIMx_IRQHANDLER 			TIM2_IRQHandler
#define TIMx_CHANNEL_x 				TIM_Channel_1
#define TIMx_IT_CCx	 					TIM_IT_CC1
#define TIM_NVIC_PREEMPTIONPRIORITY 	2
#define TIM_NVIC_SUBPRIORITY   				0

#define TIM_OCxPolarityConfig TIM_OC1PolarityConfig
#define TIM_GetCapturex				TIM_GetCapture1

#define RCC_TRIG_PERIPH 	RCC_APB2Periph_GPIOA
#define TRIG_PORT 				GPIOA
#define TRIG_PIN 					GPIO_Pin_1


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
