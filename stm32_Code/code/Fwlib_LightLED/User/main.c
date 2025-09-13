#include "stm32f10x.h"
#include "bsp_led.h"

//实现三种颜色的LED流水灯

void Delay(uint32_t time);

int main()
{
	LED_GPIO_Config();  //初始化GPIOB
	
	while(1)
	{
		#if 0
		//GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);  //设置LED_GPIO_PIN为1，关闭LED
		LED_G(OFF);  //使用宏定义的方式点亮LED
		Delay(0xFFFFF);
		//GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);  //清除LED_GPIO_PIN为0，点亮LED
		LED_G(ON);
		Delay(0xFFFFF);
		
		#elif 1
		LED_R(ON);
		LED_G(OFF);
		LED_B(OFF);
		Delay(0x5FFFFF);
		
		LED_R(OFF);
		LED_G(ON);
		LED_B(OFF);
		Delay(0x5FFFFF);
		
		LED_R(OFF);
		LED_G(OFF);
		LED_B(ON);
		Delay(0x5FFFFF);
		
		#endif
	}
	
	
	return 0;
}


void Delay(uint32_t time)
{
	while(time--);
}
