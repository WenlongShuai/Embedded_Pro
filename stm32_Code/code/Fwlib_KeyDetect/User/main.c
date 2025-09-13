#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

int main()
{
	uint8_t key = 0;
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	while(1)
	{
		//key = Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN); //检测K1按键
		key = Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN); //检测K2按键
		if(key == KEY_ON)
			//每按下KEY1按键一次，LED_G灯状态就转换一次
			LED_G_TOGGLE;
	}
	return 0;
}

