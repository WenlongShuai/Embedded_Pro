#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "bsp_led.h"

extern uint16_t adcConversionValue;

void delay(uint32_t time);

int main()
{
	float adcConversionValueLocal= 0.0f;
	
	LED_GPIO_Config();
	USART_GPIO_Init();
	ADCx_Init();
	
	printf("这是一个ADC实验，使用ADC的独立模式、单通道、中断读取ADC数据\r\n");
	LED_G(ON);
	
	while(1)
	{
		adcConversionValueLocal = adcConversionValue / 4096.0 * 3.3;
		printf("ADCx_CHANNEL_11 = 0x%04x\r\n", adcConversionValue);
		printf("ADCx_CHANNEL_11 value = %f V\r\n", adcConversionValueLocal);
		delay(0x6FFFFF);
	}
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

