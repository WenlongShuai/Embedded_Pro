#ifndef __DMA_H__
#define __DMA_H__

#include "sys.h"

//DMA
#define I2C_DMA_CLK               RCC_AHB1Periph_DMA1	
#define I2C_DMA_CHANNEL           DMA_Channel_1
#define I2C_DMA_STREAM            DMA1_Stream0

void I2C1_DMA_Init(void);
uint8_t I2C1_CheckBusState(void);
void I2C1_Reset(void);
uint8_t I2C1_DMA_SendData(uint8_t slaveAddr, uint8_t *data, uint16_t size);
uint8_t I2C1_DMA_ReceiveData(uint8_t slaveAddr,uint8_t regAddress, uint8_t *data, uint16_t size);
uint8_t I2C1_DMA_SendThenReceive(uint8_t slaveAddr, uint8_t *txData, uint16_t txSize, uint8_t *rxData, uint16_t rxSize);
uint8_t I2C1_CheckConnection(uint8_t slaveAddr);


#endif /* __DMA_H__ */