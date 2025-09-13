#include "bsp_SystemClock.h"

//使用寄存器实现系统时钟配置
//HSE作为系统时钟源
void HSE_SetSysClock_REG(uint32_t RCC_CFGR_PLLMULLx)
{
	uint32_t HSEStatus = 0, HSEStartUpCount = 0;
	
	RCC_DeInit();  //复位RCC所有寄存器
	
	RCC->CR |= RCC_CR_HSEON;  //使能HSE时钟
	
	//等待HSE时钟就绪，如果就绪超时，则不继续判断是否就绪，而直接退出
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		HSEStartUpCount++;
	}while((HSEStatus != RCC_CR_HSERDY) && (HSEStartUpCount != HSE_STARTUP_TIMEOUT));
	
	//再次判断HSE时钟是否准备就绪
	if((RCC->CR & RCC_CR_HSERDY) != 0)
	{
		//就绪完成，将HSE时钟的状态变量置1
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		//就绪未完成，将HSE时钟的状态变量置0
		HSEStatus = (uint32_t)0x00;
	}
	
	//HSE时钟就绪完成
	if(HSEStatus == 0x01)
	{
		//FLASH 开启预取缓冲区
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    //将FLASH 的时延设置为2个等待状态
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;  
		
		//选择PLL时钟输入源HSE是不分频的
		RCC->CFGR &= RCC_CFGR_PLLXTPRE_HSE;
		
		//选择PLL时钟输入源是HSE
		RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;
		
		//设置PLL时钟源输出的倍频
		RCC->CFGR |= RCC_CFGR_PLLMULLx;
		
		//使能PLL
		RCC->CR |= RCC_CR_PLLON;
		
		//等待PLL使能完成
		while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);
		
		//选择SYSCLK的时钟源，有HSI、PLLCLK、HSE三种选择
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		
		//等待SYSCLK时钟选择完成
		while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
		
		//HCLK = SYSCLK，设置AHB分频器的分频系数，HCLK=SYSCLK/1
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
		
		//PCLK1 = HCLK，设置APB1分频器的分频系数，APB1=SYSCLK/2
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
		
		//PCLK2 = HCLK,设置APB2分频器的分频系数，APB2=SYSCLK/1
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	}
	//HSE时钟就绪未完成
	else
	{
		//HSE时钟启动失败的时候，处理错误的代码
	}
}


//HSI经过2分频作为系统时钟源
void HSI_SetSysClock_REG(uint32_t RCC_CFGR_PLLMULLx)
{
	uint32_t HSIStatus = 0, HSIStartUpCount = 0;
	
	RCC_DeInit();  //复位RCC所有寄存器
	
	RCC->CR |= RCC_CR_HSION;  //使能HSI时钟
	
	//等待HSI时钟就绪，如果就绪超时，则不继续判断是否就绪，而直接退出
	do
	{
		HSIStatus = RCC->CR & RCC_CR_HSIRDY;
		HSIStartUpCount++;
	}while((HSIStatus != RCC_CR_HSIRDY) && (HSIStartUpCount != (uint16_t)0x0500));
	
	//再次判断HSI时钟是否准备就绪
	if((RCC->CR & RCC_CR_HSIRDY) != 0)
	{
		//就绪完成，将HSI时钟的状态变量置1
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		//就绪未完成，将HSI时钟的状态变量置0
		HSIStatus = (uint32_t)0x00;
	}
	
	//HSI时钟就绪完成
	if(HSIStatus == 0x01)
	{
		//FLASH 开启预取缓冲区
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    //将FLASH 的时延设置为2个等待状态
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;  
		
		//选择PLL时钟输入源是HSI经过2分频
		RCC->CFGR &= RCC_CFGR_PLLSRC_HSI_Div2;
		
		//设置PLL时钟源输出的倍频
		RCC->CFGR |= RCC_CFGR_PLLMULLx;
		
		//使能PLL
		RCC->CR |= RCC_CR_PLLON;
		
		//等待PLL使能完成
		while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);
		
		//选择SYSCLK的时钟源，有HSI、PLLCLK、HSE三种选择
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		
		//等待SYSCLK时钟选择完成
		while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
		
		//HCLK = SYSCLK，设置AHB分频器的分频系数，HCLK=SYSCLK/1
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
		
		//PCLK1 = HCLK，设置APB1分频器的分频系数，APB1=SYSCLK/2
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
		
		//PCLK2 = HCLK,设置APB2分频器的分频系数，APB2=SYSCLK/1
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	}
	//HSI时钟就绪未完成
	else
	{
		//HSI时钟启动失败的时候，处理错误的代码
	}
}

//使用固件库实现系统时钟编程
//HSE作为系统时钟源
void HSE_SysSetClock_FWLib(uint32_t RCC_PLLMul_x)
{
	ErrorStatus HSEStatus;
	
	RCC_DeInit();  //复位RCC所有寄存器
	
	RCC_HSEConfig(RCC_HSE_ON);  //使能HSE时钟
	
	//等待HSE时钟就绪，如果就绪超时，则不继续判断是否就绪，而直接退出
	HSEStatus = RCC_WaitForHSEStartUp();
	
	if(HSEStatus == SUCCESS)  //HSE 就绪完成
	{
		//FLASH 开启预取缓冲区
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		
		//设置FLASH 时延为2个等待状态
		FLASH_SetLatency(FLASH_Latency_2);
		
		//PLL选择时钟源并进行倍频
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_x);
		
		//使能PLL
		RCC_PLLCmd(ENABLE);
		
		//等待PLL使能完成
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET);
		
		//选择SYSCLK的时钟源，有HSI、PLLCLK、HSE三种选择
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//等待SYSCLK时钟选择完成
		while(RCC_GetSYSCLKSource() != 0x08);
		
		//HCLK = SYSCLK，设置AHB分频器的分频系数，HCLK=SYSCLK/1
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		
		//PCLK1 = HCLK，设置APB1分频器的分频系数，APB1=SYSCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		//PCLK2 = HCLK,设置APB2分频器的分频系数，APB2=SYSCLK/1
		RCC_PCLK2Config(RCC_HCLK_Div1);
	
	}
	else  //HSE 就绪未完成
	{
		//HSE时钟启动失败的时候，处理错误的代码
	}
}


//HSI时钟2分频后作为系统时钟源
void HSI_SysSetClock_FWLib(uint32_t RCC_PLLMul_x)
{
	uint32_t HSIStatus = 0, HSIStartUpCount = 0;
	
	RCC_DeInit();  //复位RCC所有寄存器
	
	RCC_HSICmd(ENABLE); //使能HSI时钟
	
	//等待HSI时钟就绪，如果就绪超时，则不继续判断是否就绪，而直接退出
	do
	{
		HSIStatus = RCC->CR & RCC_CR_HSIRDY;
		HSIStartUpCount++;
	}while((HSIStatus != RCC_CR_HSIRDY) && (HSIStartUpCount != (uint16_t)0x0500));
	
	//再次判断HSI时钟是否准备就绪
	if((RCC->CR & RCC_CR_HSIRDY) != 0)
	{
		//就绪完成，将HSI时钟的状态变量置1
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		//就绪未完成，将HSI时钟的状态变量置0
		HSIStatus = (uint32_t)0x00;
	}

	if(HSIStatus == 0x01)  //HSI 就绪完成
	{
		//FLASH 开启预取缓冲区
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		
		//设置FLASH 时延为2个等待状态
		FLASH_SetLatency(FLASH_Latency_2);
		
		//PLL选择时钟源并进行倍频
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_x);
		
		//使能PLL
		RCC_PLLCmd(ENABLE);
		
		//等待PLL使能完成
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET);
		
		//选择SYSCLK的时钟源，有HSI、PLLCLK、HSE三种选择
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//等待SYSCLK时钟选择完成
		while(RCC_GetSYSCLKSource() != 0x08);
		
		//HCLK = SYSCLK，设置AHB分频器的分频系数，HCLK=SYSCLK/1
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		
		//PCLK1 = HCLK，设置APB1分频器的分频系数，APB1=SYSCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		//PCLK2 = HCLK,设置APB2分频器的分频系数，APB2=SYSCLK/1
		RCC_PCLK2Config(RCC_HCLK_Div1);
	
	}
	else  //HSI 就绪未完成
	{
		//HSI时钟启动失败的时候，处理错误的代码
	}
}
