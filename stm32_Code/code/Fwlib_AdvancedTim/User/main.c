#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_advancedTim.h"

/* 使用高级定时器TIM1、TIM8完成定时 */

int main()
{
	LED_GPIO_Config();
	AdvancedTim_Init();
	
	while(1)
	{
		LED_R_TOGGLE;
		AdvancedTim_Delay_ms(1000);
	}
	
	
	return 0;
}




