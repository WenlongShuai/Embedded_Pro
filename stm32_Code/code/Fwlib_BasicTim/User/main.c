#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_basicTim.h"

/* 使用基本定时器TIM6、TIM7完成定时 */

int main()
{
	LED_GPIO_Config();
	BasicTim_Init();
	
	while(1)
	{
		LED_R_TOGGLE;
		BasicTim_Delay_ms(500);
	}
	
	
	return 0;
}




