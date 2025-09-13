#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#include "stm32f10x.h"

//SPI_GPIO
#define SPI_GPIO_CLK 						 	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC
#define SPI_GPIO_RCC_ClockCmd	 		RCC_APB2PeriphClockCmd
#define SPI_GPIO_SCK_PORT				 	GPIOA
#define SPI_GPIO_SCK_PIN			 		GPIO_Pin_5
#define SPI_GPIO_MOSI_PORT 				GPIOA
#define SPI_GPIO_MOSI_PIN 				GPIO_Pin_7
#define SPI_GPIO_MISO_PORT  			GPIOA
#define SPI_GPIO_MISO_PIN 			 	GPIO_Pin_6
#define SPI_GPIO_CS_PORT 					GPIOC
#define SPI_GPIO_CS_PIN 				 	GPIO_Pin_0

//SPI
//x:为了防止宏定义与其他文件的重复，导致报错
#define SPIx  										SPI1
#define SPIx_CLK									RCC_APB2Periph_SPI1
#define SPIx_CLK_ClockCmd  				RCC_APB2PeriphClockCmd
#define SPIx_BAUDRATEPRE  				SPI_BaudRatePrescaler_2
#define SPIx_CPHA  								SPI_CPHA_2Edge
#define SPIx_CPOL 								SPI_CPOL_High
#define SPIx_CRC									0x0007
#define SPIx_DATASIZE  						SPI_DataSize_8b
#define SPIx_DIRECTION  					SPI_Direction_2Lines_FullDuplex
#define SPIx_FIRSTBIT  						SPI_FirstBit_MSB
#define SPIx_MODE  								SPI_Mode_Master
#define SPIx_NSS 									SPI_NSS_Soft

#define DUMMY_BYTE               	0
#define FLASH_PAGE_NUM						256


#define SPI_FLASH_CS_HIGH()				GPIO_SetBits(SPI_GPIO_CS_PORT, SPI_GPIO_CS_PIN)
#define SPI_FLASH_CS_LOW()				GPIO_ResetBits(SPI_GPIO_CS_PORT, SPI_GPIO_CS_PIN)


#define FLASH_WRITEENABLE       	0x06
#define FLASH_WRITEDISABLE       	0x04
#define FLASH_READSTATUSREG1			0x05
#define FLASH_READSTATUSREG2			0x35
#define FLASH_WRITESTATUSREG			0x01
#define FLASH_PAGEPROGRAM     		0x02
#define FLASH_SECTORERASE     		0x20
#define FLASH_BLOCKERASE_32K  		0x52
#define FLASH_BLOCKERASE_64K  		0xD8
#define FLASH_CHIPERASE  					0xC7
#define FLASH_POWERDOWN   				0xB9
#define FLASH_RELEASEPOWERDOWN   	0xAB
#define FLASH_READDATA 						0x03

#define ABH_DEVICEID							0xAB
#define Manufacturer_DEVICEID    	0x90
#define Unique_DEVICEID    				0x4B
#define FLASH_JEDECID							0x9F

#define BLOCK64K          1
#define BLOCK32K          0
#define FLASH_ID          0xef4017



#define SPIx_FLAG_TIMEOUT    ((uint32_t)0x1000)
#define SPIx_LONG_TIMEOUT    ((uint32_t)(2*SPIx_FLAG_TIMEOUT))

/*信息输出*/
#define SPI_DEBUG_ON         0

#define SPI_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define SPI_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define SPI_DEBUG(fmt,arg...)          do{\
																				if(EEPROM_DEBUG_ON)\
																				printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																				}while(0)

void SPI_GPIO_Init();
uint16_t SPI_Flash_SendData(uint16_t data);
uint16_t SPI_Flash_ReceiveData();
void SPI_Flash_WriteEnable();
void SPI_Flash_WaitForBUSY();
void SPI_Flash_WriteREG(uint16_t REGData);
uint8_t SPI_Flash_ReadStatusReg();
uint8_t SPI_Flash_ReadByteData(uint32_t readAddr);
void SPI_Flash_SequenceReadData(uint32_t readAddr, uint8_t *readData, uint32_t readLen);
void SPI_Flash_PageWrite(uint32_t pageAddr, uint8_t *pageData, uint16_t pageLen);
void SPI_Flash_SequenceWriteData(uint32_t writeAddr, uint8_t *writeData, uint32_t writeLen);
void SPI_Flash_SectorErase(uint32_t sectorEraseAddr);
void SPI_Flash_BlockErase_32K_64K(uint8_t flag, uint32_t blockEraseAddr);
void SPI_Flash_ChipErase();
void SPI_Flash_PowerDown();
void SPI_Flash_ReleasePowerDown();

uint32_t SPI_Flash_Read_JEDECId();
uint8_t SPI_Flash_Read_ABHDeviceId();
uint16_t SPI_Flash_Read_ManufacturerDeviceId();
uint64_t SPI_Flash_Read_UniqueDeviceId();


#endif /* __BSP_SPI_FLASH_H__ */

