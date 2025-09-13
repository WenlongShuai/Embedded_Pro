#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_pvd.h"

/* 
stm32电源管理单元中的可编程电压检测器PVD，用于实时检测VDD的电压，
当检测到低于/高于我们设置的电压时就会产生一个PVD中断(EXTI16中断) 
*/

int main()
{
	LED_GPIO_Config();
	USART_GPIO_Init();
	pvd_Config();
	
	LED_G(ON);//正常情况亮绿灯
	
	while(1)
	{
	
	}
	
	return 0;
}

