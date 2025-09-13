#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_SystemClock.h"

//分别用寄存器操作和固件库操作对HSE、HSI进行系统时钟配置

void Delay(uint32_t time);

int main()
{
	LED_GPIO_Config();  //初始化GPIOB
	
	//HSE_SetSysClock_REG(RCC_CFGR_PLLMULL9);
	//HSI_SetSysClock_REG(RCC_CFGR_PLLMULL16);
	
	HSE_SysSetClock_FWLib(RCC_PLLMul_9);
	//HSI_SysSetClock_FWLib(RCC_PLLMul_16);
	
	LED_G(OFF);
	LED_B(OFF);
	
	while(1)
	{
		LED_R(ON);

		Delay(0x1FFFFF);
		
		LED_R(OFF);
		Delay(0x1FFFFF);
		
	}
	
	
	return 0;
}


void Delay(uint32_t time)
{
	while(time--);
}
