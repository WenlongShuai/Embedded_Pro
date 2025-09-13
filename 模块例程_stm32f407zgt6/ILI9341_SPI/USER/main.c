#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "lcd_demo.h"
#include "lcd_ili9341.h"


int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	
	Lcd_Init();
	
	while(1)
	{
			QDTFT_Test_Demo();	//See the test details in QDTFT_Demo.c		
	}
	
	return 0;
}