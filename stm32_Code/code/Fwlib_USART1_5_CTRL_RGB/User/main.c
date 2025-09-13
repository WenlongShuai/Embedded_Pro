#include "stm32f10x.h"
#include "bsp_usart1_5.h"
#include "bsp_led.h"

int main()
{
	uint8_t *str = "\n\
此项目为上位机控制STM32F103开发板的RGB\n\
---------------1:点亮红灯------------\n\
---------------2:点亮绿灯------------\n\
---------------3:点亮蓝灯------------\n\
---------------4:关闭RGB-------------\n\
---------------0:退出程序------------\n";
	
	USART_GPIO_Init();
	LED_GPIO_Config();
	
	Usart_SendStrData(USARTx, str);
	
	do
	{
		switch(receiveData)
		{
			case '1':
				LED_R(ON);
				break;
			case '2':
				LED_G(ON);
				break;
			case '3':
				LED_B(ON);
				break;
			case '4':
				LED_BGR_OFF;
				break;
		}
	}while(receiveData != '0');
	
	Usart_SendStrData(USARTx, "退出成功");
	
	return 0;
}

