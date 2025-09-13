#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_advancedTim_InputCapture_MeaPWM.h"
#include "bsp_generalTim_Output_PWM.h"


/* 使用高级定时器TIM1、TIM8实现输入PWM信号捕获，算出PWM的占空比和频率 */

extern ICxCountValueStruct ICxCountValue;

int main()
{
	float DutyCycle = 0;
	float Frequency = 0;
	float singleCountFre = 72000000 / (ADVANCEDTIM_PRESCALER+1);
	
	USART_GPIO_Init();
	LED_GPIO_Config();
	GeneralTim_Init();
	AdvancedTim_Init();
	
	LED_B(ON);
	
	while(1)
	{
		if(ICxCountValue.IC1Value != 0)
		{
			printf("IC1Value = %d  IC2Value = %d ",ICxCountValue.IC1Value,ICxCountValue.IC2Value);
			
			Frequency = singleCountFre / (ICxCountValue.IC1Value+1);  //Hz
			
			DutyCycle = ((ICxCountValue.IC2Value+1)*100) / (ICxCountValue.IC1Value+1);
			
			printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
			
		}
		else
		{
			Frequency = 0;
			DutyCycle = 0;
		}
	}
	
	
	return 0;
}




