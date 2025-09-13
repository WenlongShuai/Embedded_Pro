#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_AdvancedTim_ComplementaryOutput.h"


/* 使用高级定时器TIM1、TIM8输出互补PWM，包括死区时间和刹车功能 */

int main()
{
	LED_GPIO_Config();
	AdvancedTim_Init();
	
	while(1)
	{
		
	}
	
	
	return 0;
}




