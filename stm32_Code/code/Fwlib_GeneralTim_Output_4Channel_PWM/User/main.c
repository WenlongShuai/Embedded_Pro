#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_generalTim_Output_PWM.h"


/* 使用通用定时器TIM2~TIM5输出4路不同占空比的PWM */

int main()
{
	LED_GPIO_Config();
	GeneralTim_Init();
	
	while(1)
	{
		LED_R_TOGGLE;
	}
	
	
	return 0;
}




