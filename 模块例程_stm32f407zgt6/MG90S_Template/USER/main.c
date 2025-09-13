#include "usart.h"
#include "oled.h"
#include "sys.h"
#include "mg90s.h"

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(72);
	usart_init(72, 115200);
	OLED_Init();
	MG90S_Init();
		
	OLED_ShowStr(16, 0, "freeRTOS Test", 2);
	
	MG90S_SetAngle(0);
	
	while(1)
	{
		printf("hhh\r\n");
		delay_ms(200);
	}
}
