#include "bsp_gpio_exti.h"

static void KEY_GPIO_Config()
{
	RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//KEY2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = KEY2_GPIO_PIN_EXTI;
	GPIO_Init(KEY2_GPIO_PORT_EXTI, &GPIO_InitStruct);
}

uint8_t Key2_LongPress(void)
{
	uint8_t time = 10;
	while(time--)
	{
		if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT_EXTI, KEY2_GPIO_PIN_EXTI) == RESET)
		{
			return 0;
		}
	}
	
	return 1;
}