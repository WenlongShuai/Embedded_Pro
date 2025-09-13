#include "bsp_wwdg.h"

static void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_Init(&NVIC_InitStruct);
}

void WWDG_Config(uint8_t prv, uint8_t wr, uint8_t tr)
{
	//打开窗口看门狗的时钟PCLK1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	//设置WWDG计数器值
	WWDG_SetCounter(tr);
	
	//设置窗口看门狗的预分频时基
	WWDG_SetPrescaler(prv);
	
	//设置窗口看门狗的窗口上限
	WWDG_SetWindowValue(wr);
	
	//使能窗口看门狗，并设置窗口看门狗计数器的值为0x7F
	//启用WWDG并加载计数器值
	WWDG_Enable(WWDG_CNT);
	
	//清除提前唤醒中断标志
	WWDG_ClearFlag();
	
	//设置WWDG窗口看门狗的中断优先级
	NVIC_Config();
	
	//打开提前唤醒中断
	WWDG_EnableIT();
}

void WWDG_Feed(void)
{
	//刷新递减计数器的值，喂狗操作，设置成最大的WWDG_CNT=0x7F
	WWDG_SetCounter(WWDG_CNT);
}
