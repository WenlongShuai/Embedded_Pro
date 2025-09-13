#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_gpio_exti.h"
#include "bsp_SystemClock.h"

/* 
stm32进入停止模式(深度睡眠模式)，WFI进入的停止模式只能通过任一外部中断(EXTI中断)进行唤醒
WFE进入的停止模式只能通过任一外部事件(EXTI事件)进行唤醒
*/

void delay(uint32_t time);

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	LED_GPIO_Config();
	USART_GPIO_Init();
	KEY_EXTI_Config();
	
	
	LED_B(ON);
	printf("进入stop mode之前\r\n");
	RCC_GetClocksFreq(&RCC_Clocks);
	printf("HCLK = %d M\r\n",RCC_Clocks.HCLK_Frequency / 1000000);  //72M
	printf("PCLK1 = %d M\r\n",RCC_Clocks.PCLK1_Frequency / 1000000);  //36M
	printf("PCLK2 = %d M\r\n",RCC_Clocks.PCLK2_Frequency / 1000000);  //72M
	printf("SYSCLK = %d M\r\n",RCC_Clocks.SYSCLK_Frequency / 1000000);  //72M
	
	//需要在进入停止模式之前延时一会，等待串口信息输出完成，
	//如果不延时，还没等串口输出完，stm32就进入停止模式，关闭了所有时钟
	delay(0xFFFFF);
	
	#if defined(INTERRUPT)
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); //调用WFI进入停止模式，调压器工作在正常模式
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); //调用WFI进入停止模式，调压器工作在低功耗模式
	#else
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE); //调用WFE进入停止模式，调压器工作在正常模式
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE); //调用WFE进入停止模式，调压器工作在低功耗模式
	#endif
	RCC_GetClocksFreq(&RCC_Clocks);
	//进入停止模式，被唤醒之后，系统会把内部HSI作为系统时钟，后面的程序全是以8M的速度执行。
	HSE_SysSetClock_FWLib();
	printf("进入stop mode之后\r\n");
	
	printf("HCLK = %d M\r\n",RCC_Clocks.HCLK_Frequency / 1000000);   //8M
	printf("PCLK1 = %d M\r\n",RCC_Clocks.PCLK1_Frequency / 1000000);  //4M
	printf("PCLK2 = %d M\r\n",RCC_Clocks.PCLK2_Frequency / 1000000);  //8M
	printf("SYSCLK = %d M\r\n",RCC_Clocks.SYSCLK_Frequency / 1000000);  //8M
	LED_R(ON);
	delay(0xFFFFF);
	
	while(1)
	{
		LED_G(ON);
		printf("在while中进入stop mode之前\r\n");
		//需要在进入停止模式之前延时一会，等待串口信息输出完成，
		//如果不延时，还没等串口输出完，stm32就进入停止模式，关闭了所有时钟
		delay(0xFFFFF);
		
		#if defined(INTERRUPT)
		//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); //调用WFI进入停止模式，调压器工作在正常模式
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); //调用WFI进入停止模式，调压器工作在低功耗模式
		#else
		//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE); //调用WFE进入停止模式，调压器工作在正常模式
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE); //调用WFE进入停止模式，调压器工作在低功耗模式
		#endif
		
		HSE_SysSetClock_FWLib();
		printf("在while中进入stop mode之后\r\n");
	}
	
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

