#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_dma_MToM.h"

extern const uint32_t FLASH_Data[BUFFER_SIZE];
extern uint32_t SRAM_Data[BUFFER_SIZE];

void Delay(uint32_t time);

int main()
{
	uint8_t result = 0;
	LED_GPIO_Config();
	
	//红 + 绿 = 黄
	LED_R(ON);
	LED_G(ON);
	Delay(0xFFFFFF);
	
	MToM_DMA_Config();
	
	//等到DMA传输完成
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);

	result = buffer_Compare(FLASH_Data, SRAM_Data, DMA_BUFFERSIZE);
	if(result == 0)
	{
		LED_R(ON);
		LED_G(OFF);
		LED_B(OFF);
	}
	else
	{
		LED_R(OFF);
		LED_G(ON);
		LED_B(OFF);
	}
	
	while(1)
	{
	
	}
	return 0;
}

void Delay(uint32_t time)
{
	while(time--);
}

