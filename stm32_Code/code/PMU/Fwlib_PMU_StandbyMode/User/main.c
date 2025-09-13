#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"


void delay(uint32_t time);

int main()
{
	USART_GPIO_Init();
	LED_GPIO_Config();
	
	//在进入待机模式之前，必须先打开PWR的时钟，否则就进不了待机模式
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	LED_B(ON);
	
	while(1)
	{
		if(Key2_LongPress())
		{
			LED_G(ON);
			printf("即将待机模式，进入待机模式后可以使用KEY2进行唤醒，唤醒后会进行复位，程序从头开始执行\r\n");
			delay(0xFFFFF);
			PWR_ClearFlag(PWR_FLAG_WU); //清除WUF标志位
			PWR_WakeUpPinCmd(ENABLE);  //使能WKUP(PA0)引脚，这里使用PA0的时候，不需要对PA0做任何处理，默认就是WKUP功能
			PWR_EnterSTANDBYMode(); //进入待机模式，后面唤醒后，程序重新从启动文件开始执行，相当于才上电一样
		}
		else
		{
			LED_R(ON);
			printf("KEY2按下时间不够长，需要长按才能进入待机模式\r\n");
		}
	}
	
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}
