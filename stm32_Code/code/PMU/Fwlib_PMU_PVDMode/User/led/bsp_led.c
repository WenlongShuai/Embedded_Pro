#include "bsp_led.h"

void LED_GPIO_Config(void)
{
	//1、打开GPIO的时钟(APB2)，配置APB2寄存器
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	
	//2、显示初始化GPIO(输入输出模式、速度)，配置CRL寄存器
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED_R_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED_B_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	LED_BGR_OFF;
}
