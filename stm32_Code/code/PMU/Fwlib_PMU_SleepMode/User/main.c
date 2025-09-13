#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"

/* 通过任一中断或者唤醒事件对stm32进行睡眠模式唤醒 */

int main()
{
	LED_GPIO_Config();
	USART_GPIO_Init();
	KEY_EXTI_Config();
	
	LED_B(ON);
	/* 睡眠模式，通过任一中断唤醒 */
//	printf("在WFI命令之前\r\n");
//	__WFI();
//	printf("在WFI命令之后\r\n");
	
	/* 睡眠模式，通过唤醒事件唤醒 */
	printf("在WFE命令之前\r\n");
	__WFE();
	printf("在WFE命令之后\r\n");
	LED_G(ON);
	
	while(1)
	{
//		printf("在while中WFI命令之前\r\n");
//		__WFI();
//		printf("在while中WFI命令之后\r\n");
		
		printf("在while中WFE命令之前\r\n");
		__WFE();
		printf("在while中WFE命令之后\r\n");
	}
	return 0;
}

