#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_dac.h"

/*
stm32的DAC双通道产生正弦波，该实验还没通过验证，因为没有示波器
正弦波的频率为：
fsin = 1/Tsin = 1 / (Tsystick*(TIM_Prescaler+1)*(TIM_Period+1)*N)
N:正弦波单个周期的点数
*/

void delay(uint32_t time);

int main()
{
	LED_GPIO_Config();
	DACx_Init();
	
	LED_G(ON);
	
	while(1)
	{
	}
	
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

