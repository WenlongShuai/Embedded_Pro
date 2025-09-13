#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "systick.h"
#include "hip600d.h"

int main()
{
	SysTick_Init();
	uart_init(115200);
	hip600d_USART_GPIO_Init(115200);
	
	while(1)
	{
		if(HIP_USART_RX_STA_FRAM != 0)
		{
			HIP_USART_RX_STA_FRAM = 0;
			HIP_USART_RX_STA_COUNT = 0;
			parseNavigationData(HIP_USART_RX_BUF);
		}
		Delay_ms(50);
	}
	
	return 0;
}

