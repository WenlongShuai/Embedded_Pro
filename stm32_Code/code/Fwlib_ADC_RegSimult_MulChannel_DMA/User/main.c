#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "bsp_led.h"

extern uint32_t adcConversionValue[ADCx_1_2_CHANNEL_NUM];

void delay(uint32_t time);

int main()
{
	float adcConversionValueLocal[ADCx_1_2_CHANNEL_NUM*2]= {0.0f};
	
	USART_GPIO_Init();
	LED_GPIO_Config();
	ADCx_Init();
	
	printf("这是一个ADC实验，使用ADC双重模式、规则同步，多通道、DMA读取ADC数据\r\n");
	LED_G(ON);
	
	while(1)
	{
		adcConversionValueLocal[0] = (adcConversionValue[0] & 0xFFFF) / 4096.0 * 3.3;  //ADC1  ADCx_1_2_CHANNEL11
		adcConversionValueLocal[1] = ((adcConversionValue[0] & 0xFFFF0000)>>16) / 4096.0 * 3.3;  //ADC2  ADCx_1_2_CHANNEL13
		
		adcConversionValueLocal[2] = (adcConversionValue[1] & 0xFFFF) / 4096.0 * 3.3;  //ADC1   ADCx_1_2_CHANNEL12
		adcConversionValueLocal[3] = ((adcConversionValue[1] & 0xFFFF0000)>>16) / 4096.0 * 3.3;  //ADC2   ADCx_1_2_CHANNEL15
		
		adcConversionValueLocal[4] = (adcConversionValue[2] & 0xFFFF) / 4096.0 * 3.3;  //ADC1   ADCx_1_2_CHANNEL13
		adcConversionValueLocal[5] = ((adcConversionValue[2] & 0xFFFF0000)>>16) / 4096.0 * 3.3;  //ADC2   ADCx_1_2_CHANNEL14

		printf("\r\n");
		printf("ADCx_1_CHANNEL_11 value = %f V\r\n", adcConversionValueLocal[0]);
		printf("ADCx_1_CHANNEL_12 value = %f V\r\n", adcConversionValueLocal[2]);
		printf("ADCx_1_CHANNEL_13 value = %f V\r\n", adcConversionValueLocal[4]);
		
		printf("ADCx_2_CHANNEL_14 value = %f V\r\n", adcConversionValueLocal[5]);
		printf("ADCx_2_CHANNEL_15 value = %f V\r\n", adcConversionValueLocal[3]);
		printf("ADCx_2_CHANNEL_13 value = %f V\r\n", adcConversionValueLocal[1]);
		
		delay(0x6FFFFF);
	}
	return 0;
}

void delay(uint32_t time)
{
	while(time--);
}

