#include "bsp_gpio_exti.h"


static void KEY_NVIC_Config()
{
	NVIC_InitTypeDef NVICInitStruct;
	
	//KEY1
	NVICInitStruct.NVIC_IRQChannel = KEY1_NVIC_IRQCHANNEL;
	NVICInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = KEY1_NVIC_IRQCHANNEL_PREPRIORITY;  //主优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = KEY1_NVIC_IRQCHANNEL_SUBPRIORITY;    //子优先级
	NVIC_PriorityGroupConfig(KEY1_NVIC_PRIORITY_GROUP);  //配置中断优先级分组
	NVIC_Init(&NVICInitStruct);
	
	//KEY2
	NVICInitStruct.NVIC_IRQChannel = KEY2_NVIC_IRQCHANNEL;
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = KEY2_NVIC_IRQCHANNEL_PREPRIORITY;  //主优先级
	NVICInitStruct.NVIC_IRQChannelSubPriority = KEY2_NVIC_IRQCHANNEL_SUBPRIORITY;    //子优先级
	NVIC_PriorityGroupConfig(KEY2_NVIC_PRIORITY_GROUP);  //配置中断优先级分组
	NVIC_Init(&NVICInitStruct);
}


static void KEY_GPIO_Config()
{
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//KEY1
	GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_PIN_EXTI;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY1_GPIO_PORT_EXTI, &GPIO_InitStruct);
	
	//KEY2
	GPIO_InitStruct.GPIO_Pin = KEY2_GPIO_PIN_EXTI;
	GPIO_Init(KEY2_GPIO_PORT_EXTI, &GPIO_InitStruct);
}

void KEY_EXTI_Config()
{
	EXTI_InitTypeDef EXTIInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //打开AFIO的时钟
	
	#if defined(INTERRUPT)
	/* 如果要使用中断唤醒睡眠模式的话，需要把stm32f10x_it.c文件的中断服务函数给打开 */
	//KEY1，中断模式
	EXTIInitStruct.EXTI_Line = KEY1_EXTI_LINE;
	EXTIInitStruct.EXTI_Mode = KEY1_EXTI_MODE;
	EXTIInitStruct.EXTI_Trigger = KEY1_EXTI_TRIGGER;
	EXTIInitStruct.EXTI_LineCmd = ENABLE;
	GPIO_EXTILineConfig(KEY1_EXTI_LINE_GPIO_PORT, KEY1_EXTI_LINE_GPIO_PIN);  //配置输入线AFIO为PA0引脚
	EXTI_Init(&EXTIInitStruct); //使能中断请求(触发选择寄存器、软件中断事件触发器、请求挂起寄存器、中断屏蔽寄存器)

	//KEY2，中断模式
	EXTIInitStruct.EXTI_Line = KEY2_EXTI_LINE;
	EXTIInitStruct.EXTI_Mode = KEY2_EXTI_MODE;
	EXTIInitStruct.EXTI_Trigger = KEY2_EXTI_TRIGGER;
	GPIO_EXTILineConfig(KEY2_EXTI_LINE_GPIO_PORT, KEY2_EXTI_LINE_GPIO_PIN);  //配置输入线AFIO为PA0引脚
	EXTI_Init(&EXTIInitStruct); //使能中断请求(触发选择寄存器、软件中断事件触发器、请求挂起寄存器、中断屏蔽寄存器)
	
	
	#else
	//KEY1，事件模式
	EXTIInitStruct.EXTI_Line = KEY1_EXTI_LINE;
	EXTIInitStruct.EXTI_Mode = EXTI_Mode_Event;
	EXTIInitStruct.EXTI_Trigger = KEY1_EXTI_TRIGGER;
	EXTIInitStruct.EXTI_LineCmd = ENABLE;
	GPIO_EXTILineConfig(KEY1_EXTI_LINE_GPIO_PORT, KEY1_EXTI_LINE_GPIO_PIN);  //配置输入线AFIO为PA0引脚
	EXTI_Init(&EXTIInitStruct); //使能中断请求(触发选择寄存器、软件中断事件触发器、请求挂起寄存器、中断屏蔽寄存器)

	//KEY2，事件模式
	EXTIInitStruct.EXTI_Line = KEY2_EXTI_LINE;
	EXTIInitStruct.EXTI_Mode = EXTI_Mode_Event;
	EXTIInitStruct.EXTI_Trigger = KEY2_EXTI_TRIGGER;
	GPIO_EXTILineConfig(KEY2_EXTI_LINE_GPIO_PORT, KEY2_EXTI_LINE_GPIO_PIN);  //配置输入线AFIO为PA0引脚
	EXTI_Init(&EXTIInitStruct); //使能中断请求(触发选择寄存器、软件中断事件触发器、请求挂起寄存器、中断屏蔽寄存器)
	
	#endif
	
	KEY_NVIC_Config();
	KEY_GPIO_Config();
	
}



