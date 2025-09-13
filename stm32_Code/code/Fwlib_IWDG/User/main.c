#include "stm32f10x.h"
#include "bsp_iwdg.h"
#include "bsp_led.h"
#include "bsp_key.h"

/*
stm32独立看门狗，看门狗溢出时间为1S，通过IWDG_Config函数的参数控制溢出函数。
如果是看门狗复位就亮红灯，上电复位或者按键复位就亮蓝灯。
如果在看门狗溢出时间范围之内进行喂狗，就会亮绿灯。
*/

void delay(uint32_t time);

int main()
{
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	delay(0x5FFFFF);
	
	//看门狗复位
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		RCC_ClearFlag();  //清除复位标志
		LED_R(ON);	
	}
	else
	{
		//不是独立看门狗复位，可能是上电复位或者按键复位
		LED_B(ON);
	}
	
	//打开看门狗配置
	IWDG_Config(IWDG_Prescaler_16, 0x9C4);
	
	while(1)
	{
		//通过KEY1进行喂狗
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			IWDG_Feed();
			LED_G(ON);
		}
	}
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

