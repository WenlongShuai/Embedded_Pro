#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_spi_flash.h"

#define FLASH_BASE 						0x000000
#define FLASH_ERASE_ADDR   		FLASH_BASE
#define FLASH_PAGEWRITE_ADDR 	FLASH_BASE
#define FLASH_PAGEREAD_ADDR  	FLASH_BASE
#define FLASH_WRITESIZE  			256
#define FLASH_READSIZE  			256

uint8_t pageWriteBuffer[FLASH_WRITESIZE] = {0};
uint8_t pageReadBuffer[FLASH_READSIZE] = {0};

uint8_t bufferCompare(uint8_t *writeBuffer, uint8_t *readBuffer, uint16_t len);

int main()
{
	uint32_t flashId = 0;
	uint16_t i = 0;
	uint8_t flag = 0;
	
	LED_GPIO_Config();
	USART_GPIO_Init();
	
	
	printf("\r\n这是一个 8MB 串行 flash(W25Q64) 实验\r\n");
	SPI_GPIO_Init();
	
	for(i = 0;i < FLASH_WRITESIZE;i++)
	{
		pageWriteBuffer[i] = i;
	}
	
	SPI_Flash_ReleasePowerDown();  //防止flash处于掉电模式
	flashId = SPI_Flash_Read_JEDECId();
	
	printf("\r\nflash ID:0x%x\n", flashId);
	
	if(flashId == FLASH_ID)
	{
		printf("\r\n检测到串行 flash(W25Q64)\r\n");
		
		SPI_Flash_SectorErase(FLASH_ERASE_ADDR);
		
		SPI_Flash_SequenceWriteData(FLASH_PAGEWRITE_ADDR, pageWriteBuffer, FLASH_WRITESIZE);
		printf("\r\n写入的数据为:\r\n");
		for(i = 0;i < FLASH_WRITESIZE;i++)
		{
			printf("0x%02x ",pageWriteBuffer[i]);
			if(i % 10 == 0)
				printf("\r\n");
		}
		
		SPI_Flash_SequenceReadData(FLASH_PAGEREAD_ADDR, pageReadBuffer, FLASH_READSIZE);
		printf("\r\n读取的数据为:%s\r\n",pageReadBuffer);
		for(i = 0;i < FLASH_READSIZE;i++)
		{
			printf("0x%02x ",pageReadBuffer[i]);
			if(i % 10 == 0)
				printf("\r\n");
		}
		
		flag = bufferCompare(pageWriteBuffer, pageReadBuffer, FLASH_WRITESIZE);
		
		if(!flag)
		{
			LED_G(ON);
			printf("\r\n8MB 串行 flash(W25Q64) 测试成功\r\n");
		}
		else
		{
			LED_R(ON);
			printf("\r\n8MB 串行 flash(W25Q64) 测试失败\r\n");
		}
	}
	else
	{
		printf("\r\n获取不到 8MB 串行 flash(W25Q64) ID\r\n");
	}
	
	while(1)
	{
	
	}
	return 0;
}


uint8_t bufferCompare(uint8_t *writeBuffer, uint8_t *readBuffer, uint16_t len)
{
	if(writeBuffer == NULL || readBuffer == NULL)
	{
		printf("writeBuffer or readBuffer is NULL\r\n");
		return 1;
	}
	
	while(len--)
	{
		if(*writeBuffer != *readBuffer)
		{
			return 1;
		}
		writeBuffer++;
		readBuffer++;
	}
	return 0;
}
