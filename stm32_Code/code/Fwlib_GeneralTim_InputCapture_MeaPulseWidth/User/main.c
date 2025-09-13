#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_generalTim_InputCapture_MeaPulseWidth.h"


/* 使用通用定时器TIM2~TIM5获取脉冲宽度，为了实验方便，此次使用的是TIM5的Channel1的PA0，直接检测KEY1按键按下的时间（脉冲宽度） */


void delay(uint32_t time);

int main()
{
	uint32_t time = 0;
	uint16_t singleCountTime = (GENERALTIM_PRESCALER + 1) / 72;  //us，定时器计数器计数一次的时间
	
	LED_GPIO_Config();
	USART_GPIO_Init();
	GeneralTim_Init();
	
	LED_G(ON);
	
	printf ( "\r\nSTM32 输入捕获实验\r\n" );
	printf ( "\r\n按下K1，测试K1按下的时间\r\n" );
	
	while(1)
	{
		if(TIM_ICUserValueStructure.Capture_FinishFlag == 1)
		{
			time = (TIM_ICUserValueStructure.Capture_Period*(GENERALTIM_PERIOD+1)+
						 TIM_ICUserValueStructure.Capture_CcrValue+1)*singleCountTime;
			
			// 打印高电平脉宽时间
			printf ( "\r\n测得高电平脉宽时间：%f s\r\n",time/1000000.0);
			
			TIM_ICUserValueStructure.Capture_FinishFlag = 0;
		}
	}
	
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}




