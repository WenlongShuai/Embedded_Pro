#ifndef __SPI_H__
#define __SPI_H__

#include "sys.h"

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


void SPIx_Init(void);			 //初始化SPI1口
u8 SPI_SendByte(u8 byte);
u16 SPI_SendHalfWord(u16 HalfWord);
uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

#endif /* __SPI_H__ */
