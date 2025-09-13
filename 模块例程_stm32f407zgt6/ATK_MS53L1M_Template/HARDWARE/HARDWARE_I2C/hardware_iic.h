#ifndef __HARDWARE_IIC_H__
#define __HARDWARE_IIC_H__

#include "sys.h"
#include "delay.h"

/*等待超时时间*/
#define HARDWARE_IICx_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define HARDWARE_IICx_LONG_TIMEOUT         ((uint32_t)(10 * HARDWARE_IICx_FLAG_TIMEOUT))

/**************************I2C参数定义，I2C1和I2C2********************************/
//驱动IMU
#define             HARDWARE_IICx_I2Cx                            I2C2
#define             HARDWARE_IICx_CLK                             RCC_APB1Periph_I2C2
#define             HARDWARE_IICx_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             HARDWARE_IICx_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd

#define             HARDWARE_IICx_SCL_GPIO_CLK                    RCC_APB2Periph_GPIOB     
#define             HARDWARE_IICx_SCL_PORT                        GPIOB   
#define             HARDWARE_IICx_SCL_PIN                         GPIO_Pin_10
#define             HARDWARE_IICx_SDA_GPIO_CLK                    RCC_APB2Periph_GPIOB     
#define             HARDWARE_IICx_SDA_PORT                        GPIOB
#define             HARDWARE_IICx_SDA_PIN                         GPIO_Pin_11

/* STM32 I2C 快速模式 */
#define HARDWARE_IICx_Speed              400000  //*

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define HARDWARE_IICx_OWN_ADDRESS7      0X0A


void Hardware_IIC_Init(void);
uint8_t Hardware_Write_shortByte(uint8_t ADDR, uint8_t reg, uint8_t REG_data);
uint8_t Hardware_Write_longByte(uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t Hardware_Read_Byte(uint8_t ADDR,uint8_t reg, uint8_t len, uint8_t *buf);
void Hardware_bitchange(uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data);
uint8_t Hardware_bitschange(uint8_t ADDR, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t Hardware_Write_commendByte(uint8_t ADDR, uint8_t reg);

uint8_t Hardware_Read_nbyte(uint8_t dev_address, uint16_t data, uint8_t len, uint8_t *rbuf);
uint8_t Hardware_Write_nbyte(uint8_t dev_address, uint16_t address, uint8_t len, uint8_t *wbuf);


#endif /* __IIC_H__ */
