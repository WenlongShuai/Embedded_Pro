#include "key.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "semphr.h"  // 信号量相关类型和函数的声明


// 全局信号量（用于中断与任务间同步）
SemaphoreHandle_t xCreateTaskSemaphore = NULL;


/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);                      //使能 PORTE 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //上拉输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN; 			//PE4--K0   PE3--K1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(KEY0_GPIO_PORT,&GPIO_InitStructure);
	
  // 将GPIO连接到EXTI线路（STM32F4使用SYSCFG配置）
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	// 配置EXTI
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// 配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// 中断处理函数
void EXTI4_IRQHandler(void) 
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		// 按键消抖（软件延时）
		while(GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)){};  //读取按键电平
		delay_ms(10);   //延时消抖
		while(GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)){};  //读取按键电平
//		// 释放信号量（中断安全的API）
		xSemaphoreGiveFromISR(
				xCreateTaskSemaphore,
				&xHigherPriorityTaskWoken
		);

		// 清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
	
	// 触发上下文切换
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
