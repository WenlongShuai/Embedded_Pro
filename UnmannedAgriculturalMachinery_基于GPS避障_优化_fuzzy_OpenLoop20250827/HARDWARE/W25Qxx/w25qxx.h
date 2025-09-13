#ifndef __W25Qxx_H__
#define __W25Qxx_H__

#include "delay.h"
#include "track.h"
#include "sys.h"

typedef struct{
	double longitude;
	double latitude;
}WGS84_T;

typedef union{
	uint8_t data[40];
	WGS84_T wgs84;
}POSI_UNION;

#define SPIx 									SPI1
#define SPI_RCC_CLK 					RCC_APB2Periph_SPI1
#define SPI_AF_SPIx						GPIO_AF_SPI1

#define SPI_SCK_PORT					GPIOB
#define SPI_SCK_PIN						GPIO_Pin_3
#define SPI_SCK_CLK           RCC_AHB1Periph_GPIOB
#define SPI_AF_SCK_PinSourcex	GPIO_PinSource3

#define SPI_MISO_PORT 				GPIOB
#define SPI_MISO_PIN					GPIO_Pin_4
#define SPI_MISO_CLK          RCC_AHB1Periph_GPIOB
#define SPI_AF_MISO_PinSourcex	GPIO_PinSource4

#define SPI_MOSI_PORT 				GPIOB
#define SPI_MOSI_PIN					GPIO_Pin_5
#define SPI_MOSI_CLK          RCC_AHB1Periph_GPIOB
#define SPI_AF_MOSI_PinSourcex	GPIO_PinSource5

#define SPI_CS_PIN            GPIO_Pin_14               
#define SPI_CS_PORT           GPIOB                     
#define SPI_CS_CLK            RCC_AHB1Periph_GPIOB

#define SPI_CS_LOW()      		{SPI_CS_PORT->BSRRH=SPI_CS_PIN;}
#define SPI_CS_HIGH()     		{SPI_CS_PORT->BSRRL=SPI_CS_PIN;}

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*信息输出*/
#define FLASH_DEBUG_ON         1

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

											

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
#define  sFLASH_ID                       0xEF4015	    //W25Q16
//#define  sFLASH_ID                       0XEF4017     //W25Q64
//#define  sFLASH_ID                       0XEF4018     //W25Q128

//#define SPI_FLASH_PageSize            4096
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
								
#define FLASH_SECTOR_SIZE 							0x1000																					
#define FLASH_SECTORx(n)								n*FLASH_SECTOR_SIZE																									

/* Private define ------------------------------------------------------------*/
/* FLASH 寄存器地址 */
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		  	0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	  	0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF

void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
u8 SPI_FLASH_ReadByte(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

void automatic_Collect(const double *collectGPSData);
void manual_Collect(const double *collectGPSData);
void Flash_Read(void);
void Flash_Get_POSI(POSI_ST* posi,uint16_t i);
void Get_Num(uint8_t* n);

#endif /* __W25Qxx_H__ */
