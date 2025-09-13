#include "stm32f10x.h"
#include "bsp_wwdg.h"
#include "bsp_led.h"
#include "bsp_usart.h"

/*
stm32窗口看门狗，红灯被点亮，一段时间后熄灭，如果之后红灯一直没被点亮，说明系统没有产生复位，
如果产生复位则红灯会被点亮。中断服务函数中的蓝灯没有被点亮，说明喂狗正常。
假设需要检测的程序执行的时间为20ms
*/

void delay(uint32_t time);

int main()
{
	uint8_t wwdg_wr = 0;
	uint8_t wwdg_tr = 0;
	uint32_t i = 0;
	
	LED_GPIO_Config();
	USART_GPIO_Init();
	
	LED_R(ON);
	
	delay(0x5FFFFF);
	
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)
	{
		RCC_ClearFlag();
		printf("窗口看门狗产生的复位\r\n");
	}
	else
	{
		printf("上电复位或者按键复位\r\n");
	}
	
	//T = Tpclk1 * 4096 * 2^WGDTB
	//WGDTB = 0 --> T = 113us
	//WGDTB = 1 --> T = 227us
	//WGDTB = 2 --> T = 455us
	//WGDTB = 3 --> T = 910us
	//窗口看门狗的窗口上限的时间稍微小于监测程序执行的时间，(TR-WR)*T < 20ms，TR = 0x7F,WR < 0x7F
	WWDG_Config(WWDG_Prescaler_1, 0x55, 0x7F);
	
	wwdg_wr = WWDG->CFR & 0x7F;   //取出窗口上限的值
	
	LED_R(OFF);
	
	while(1)
	{
		/*-------------------------------------------*/
		
		/* 这部分应该写需要被WWDG监测的程序，这段程序的运行时间决定了窗口值应该设置成多大 */
		
		/*-------------------------------------------*/
		
		wwdg_tr = WWDG->CR & 0x7F;  //取出当前计数器的值
		if(wwdg_tr < wwdg_wr)
		{
			//当计数器的值小于窗口上限值时，就执行喂狗操作
			WWDG_Feed();
		}
		
	}
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

