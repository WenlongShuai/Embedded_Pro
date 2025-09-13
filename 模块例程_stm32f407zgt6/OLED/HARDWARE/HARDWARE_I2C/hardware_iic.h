#ifndef __HARDWARE_IIC_H__
#define __HARDWARE_IIC_H__

#include "sys.h"


/**************************I2C参数定义，I2C1和I2C2********************************/
#define             IIC2_I2Cx                                I2C3
#define             IIC2_I2C_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             IIC2_I2C_CLK                             RCC_APB1Periph_I2C3
#define             IIC2_I2C_GPIO_APBxClock_FUN              RCC_AHB1PeriphClockCmd
#define             IIC2_I2C_SCL_GPIO_CLK                    RCC_AHB1Periph_GPIOA     
#define             IIC2_I2C_SCL_PORT                        GPIOA  
#define             IIC2_I2C_SCL_PIN                         GPIO_Pin_8 
#define             IIC2_I2C_SDA_GPIO_CLK                    RCC_AHB1Periph_GPIOC 
#define             IIC2_I2C_SDA_PORT                        GPIOC
#define             IIC2_I2C_SDA_PIN                         GPIO_Pin_9

/* STM32 I2C 快速模式 */
#define I2C_Speed              100000  //*

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7      0X0B


/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


void I2C_Mode_Config(void);

#endif /* __IIC_H__ */
