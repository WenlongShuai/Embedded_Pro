#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_dma_MToP.h"

extern uint8_t memoryData[BUFFER_SIZE];

void Delay(uint32_t time);

int main()
{
	uint16_t i = 0;
	
	for(i = 0;i < BUFFER_SIZE;i++)
	{
		memoryData[i] = 'S';
	}
	
	LED_GPIO_Config();
	USART_DMA_Config();
	MToP_DMA_Config();

	//USART1向DMA发送 串口发送 请求
	USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
	
	
	//DMA 向USART1发送数据的时候，不会占用CPU，此时CPU可以继续干自己的事情，就像开了多线程一样
	while(1)
	{
		LED_R_TOGGLE;
		Delay(0xFFFFF);
	}
	return 0;
}

void Delay(uint32_t time)
{
	while(time--);
}

