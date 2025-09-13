#include "bsp_pvd.h"

void pvd_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  //打开电源管理单元的时钟
	
	NVIC_InitStruct.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line16;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	PWR_PVDLevelConfig(PWR_PVDLevel_2V3);  //VDD/VDDA低于由选定的PVD阀值产生中断
	PWR_PVDCmd(ENABLE);  //使能PVD输出
}

