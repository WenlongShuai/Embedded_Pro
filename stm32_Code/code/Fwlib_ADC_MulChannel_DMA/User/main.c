#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "bsp_led.h"

extern uint16_t adcConversionValue[ADCx_CHANNEL_NUM];

void delay(uint32_t time);

int main()
{
	float adcConversionValueLocal[ADCx_CHANNEL_NUM]= {0.0f};
	
	USART_GPIO_Init();
	LED_GPIO_Config();
	ADCx_Init();
	
	printf("这是一个ADC实验，使用ADC的独立模式、多通道、DMA读取ADC数据\r\n");
	LED_G(ON);
	
	while(1)
	{
		adcConversionValueLocal[0] = adcConversionValue[0] / 4096.0 * 3.3;
		adcConversionValueLocal[1] = adcConversionValue[1] / 4096.0 * 3.3;
		adcConversionValueLocal[2] = adcConversionValue[2] / 4096.0 * 3.3;
		adcConversionValueLocal[3] = adcConversionValue[3] / 4096.0 * 3.3;
		adcConversionValueLocal[4] = adcConversionValue[4] / 4096.0 * 3.3;
		adcConversionValueLocal[5] = adcConversionValue[5] / 4096.0 * 3.3;
		
		printf("\r\n------------------------------------\r\n");
		printf("ADCx_CHANNEL_10 value = %f V\r\n", adcConversionValueLocal[5]);
		printf("ADCx_CHANNEL_11 value = %f V\r\n", adcConversionValueLocal[4]);
		printf("ADCx_CHANNEL_12 value = %f V\r\n", adcConversionValueLocal[3]);
		printf("ADCx_CHANNEL_13 value = %f V\r\n", adcConversionValueLocal[2]);
		printf("ADCx_CHANNEL_14 value = %f V\r\n", adcConversionValueLocal[1]);
		printf("ADCx_CHANNEL_15 value = %f V\r\n", adcConversionValueLocal[0]);
		
		delay(0x6FFFFF);
	}
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

