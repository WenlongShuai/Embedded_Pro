#include "bsp_SystemClock.h"


//使用固件库实现系统时钟编程
//HSE作为系统时钟源
void HSE_SysSetClock_FWLib()
{
	ErrorStatus HSEStatus;
	RCC_HSEConfig(RCC_HSE_ON);  //使能HSE时钟
	
	//等待HSE时钟就绪，如果就绪超时，则不继续判断是否就绪，而直接退出
	HSEStatus = RCC_WaitForHSEStartUp();
	
	if(HSEStatus == SUCCESS)  //HSE 就绪完成
	{
		//使能PLL
		RCC_PLLCmd(ENABLE);
		
		//等待PLL使能完成
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET);
		
		//选择SYSCLK的时钟源，有HSI、PLLCLK、HSE三种选择
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//等待SYSCLK时钟选择完成
		while(RCC_GetSYSCLKSource() != 0x08);
	}
}

