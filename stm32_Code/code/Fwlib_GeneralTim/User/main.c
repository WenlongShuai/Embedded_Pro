#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_generalTim.h"

/* 使用通用定时器TIM2~TIM5完成定时 */
extern uint16_t time;
int main()
{
	LED_GPIO_Config();
	GeneralTim_Init();
	
	while(1)
	{
		LED_R_TOGGLE;
		GeneralTim_Delay_ms(100);
	}
	
	
	return 0;
}




