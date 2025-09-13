#ifndef __BSP_I2C_EEPROM_H__
#define __BSP_I2C_EEPROM_H__

#include "stm32f10x.h"

#define EEPROM_BYTE_ALIG    8

#define I2C_GPIO_CLK     	 	RCC_APB2Periph_GPIOB
#define I2C_GPIO_SCL_PORT  	GPIOB
#define I2C_GPIO_SCL_PIN  	GPIO_Pin_6
#define I2C_GPIO_SDA_PORT 	GPIOB
#define I2C_GPIO_SDA_PIN  	GPIO_Pin_7

#define DEBUG_I2C 					I2C1
#define I2C_CLK  						RCC_APB1Periph_I2C1
#define I2C_OWNADDRESS1     0x77
#define I2C_MODE						I2C_Mode_I2C
#define I2C_DUTYCYCLE  			I2C_DutyCycle_2
#define I2C_CLOCKSPEED  		400000
#define I2C_ACKNOWLEDGEDADDRESS  I2C_AcknowledgedAddress_7bit
#define I2C_ACK  					 	I2C_Ack_Enable


#define I2Cx_FLAG_TIMEOUT    ((uint32_t)0x1000)
#define I2Cx_LONG_TIMEOUT    ((uint32_t)(2*I2Cx_FLAG_TIMEOUT))

/*ÐÅÏ¢Êä³ö*/
#define EEPROM_DEBUG_ON         0

#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

void I2C_EEPROM_Init();
void I2C_EEPROM_WriteByte(uint8_t writeAddr, uint8_t data);
void I2C_EEPROM_WritePage(uint8_t writeAddr, uint8_t *dataArray, uint8_t dataLen);
void I2C_EEPROM_WriteData(uint8_t writeAddr, uint8_t *data, uint16_t dataLen);

uint8_t I2C_EEPROM_ReadCurrent();
uint8_t I2C_EEPROM_ReadRandom(uint8_t readAddr);
void I2C_EEPROM_ReadSequential(uint8_t readAddr,uint8_t *readData , uint16_t readLen);
void EEPROM_WaitForWriteEnd();
#endif /* __BSP_I2C_EEPROM_H__ */
