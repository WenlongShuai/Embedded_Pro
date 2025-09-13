#ifndef __HARDWARE_IIC_H__
#define __HARDWARE_IIC_H__

#include "sys.h"
#include "delay.h"

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/**************************I2C参数定义，I2C1和I2C2********************************/
//驱动IMU
#define             IIC1_I2Cx                                I2C1
#define             IIC1_I2C_CLK                             RCC_APB1Periph_I2C1
#define             IIC1_I2C_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             IIC1_I2C_GPIO_APBxClock_FUN              RCC_AHB1PeriphClockCmd

#define             IIC1_I2C_SCL_GPIO_CLK                    RCC_AHB1Periph_GPIOB     
#define             IIC1_I2C_SCL_PORT                        GPIOB   
#define             IIC1_I2C_SCL_PIN                         GPIO_Pin_6
#define             IIC1_I2C_SDA_GPIO_CLK                    RCC_AHB1Periph_GPIOB     
#define             IIC1_I2C_SDA_PORT                        GPIOB
#define             IIC1_I2C_SDA_PIN                         GPIO_Pin_7

//驱动OLED
#define             IIC2_I2Cx                                I2C2
#define             IIC2_I2C_CLK                             RCC_APB1Periph_I2C2
#define             IIC2_I2C_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             IIC2_I2C_GPIO_APBxClock_FUN              RCC_AHB1PeriphClockCmd

#define             IIC2_I2C_SCL_GPIO_CLK                    RCC_AHB1Periph_GPIOF     
#define             IIC2_I2C_SCL_PORT                        GPIOF   
#define             IIC2_I2C_SCL_PIN                         GPIO_Pin_1
#define             IIC2_I2C_SDA_GPIO_CLK                    RCC_AHB1Periph_GPIOF     
#define             IIC2_I2C_SDA_PORT                        GPIOF   
#define             IIC2_I2C_SDA_PIN                         GPIO_Pin_0

/* STM32 I2C 快速模式 */
#define I2C_Speed              400000  //*

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7      0X0A


void Hardware_IIC_Init(uint8_t type);
uint8_t Hardware_Write_shortByte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t REG_data);
uint8_t Hardware_Write_longByte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t Hardware_Read_Byte(uint8_t type, uint8_t ADDR,uint8_t reg, uint8_t len, uint8_t *buf);
void Hardware_bitchange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data);
uint8_t Hardware_bitschange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t Hardware_Write_commendByte(uint8_t type, uint8_t ADDR, uint8_t reg);

#endif /* __IIC_H__ */
