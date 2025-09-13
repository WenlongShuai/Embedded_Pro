#include <stdio.h>
#include "usart.h"
#include "systick.h"
#include "bluetooth.h"
#include "usartx.h"

int main()
{
	SysTick_Init();
	uart_init(115200);
	Bluetooth_USART_GPIO_Init(9600);
	
	while(1)
	{
//		if(!set_Bluetooth_Param(6, "115200"))
//			printf("111发送成功\r\n");
		if(!set_Bluetooth_Param(6, NULL))
			printf("222发送成功\r\n");
		printf("%s\r\n",Bluetooth_USART_RX_BUF);
		Delay_ms(500);
	}
	
	return 0;
}
