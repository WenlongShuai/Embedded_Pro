#ifndef __SOFTWARE_IIC_H__
#define __SOFTWARE_IIC_H__

#include "sys.h"
#include "delay.h"

//驱动IMU
/* I2C1 引脚定义 */
#define SOFTWARE_IIC1_SCL_GPIO_RCC   					 RCC_AHB1Periph_GPIOB
#define SOFTWARE_IIC1_SCL_GPIO_PORT            GPIOB
#define SOFTWARE_IIC1_SCL_GPIO_PIN             GPIO_Pin_6
#define SOFTWARE_IIC1_SDA_GPIO_RCC   					 RCC_AHB1Periph_GPIOB
#define SOFTWARE_IIC1_SDA_GPIO_PORT            GPIOB
#define SOFTWARE_IIC1_SDA_GPIO_PIN             GPIO_Pin_7

//IO方向设置
#define SOFTWARE_IIC1_SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//输入模式
#define SOFTWARE_IIC1_SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} 	//输出模式

/* I2C1 IO操作 */
#define SOFTWARE_IIC1_SCL(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC1_SCL_GPIO_PORT, SOFTWARE_IIC1_SCL_GPIO_PIN):    \
                                                    GPIO_ResetBits(SOFTWARE_IIC1_SCL_GPIO_PORT, SOFTWARE_IIC1_SCL_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC1_SDA(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN) :    \
                                                    GPIO_ResetBits(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC1_READ_SDA()               GPIO_ReadInputDataBit(SOFTWARE_IIC1_SDA_GPIO_PORT, SOFTWARE_IIC1_SDA_GPIO_PIN)

					

//驱动OLED
/* I2C2 引脚定义 */
#define SOFTWARE_IIC2_SCL_GPIO_RCC   					 RCC_AHB1Periph_GPIOF
#define SOFTWARE_IIC2_SCL_GPIO_PORT            GPIOF
#define SOFTWARE_IIC2_SCL_GPIO_PIN             GPIO_Pin_1
#define SOFTWARE_IIC2_SDA_GPIO_RCC   					 RCC_AHB1Periph_GPIOF
#define SOFTWARE_IIC2_SDA_GPIO_PORT            GPIOF
#define SOFTWARE_IIC2_SDA_GPIO_PIN             GPIO_Pin_0

//IO方向设置
#define SOFTWARE_IIC2_SDA_IN()  {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=0<<0*2;}	//输入模式
#define SOFTWARE_IIC2_SDA_OUT() {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=1<<0*2;} 	//输出模式
																								
/* I2C2 IO操作 */
#define SOFTWARE_IIC2_SCL(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC2_SCL_GPIO_PORT, SOFTWARE_IIC2_SCL_GPIO_PIN):    \
                                                    GPIO_ResetBits(SOFTWARE_IIC2_SCL_GPIO_PORT, SOFTWARE_IIC2_SCL_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC2_SDA(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC2_SDA_GPIO_PORT, SOFTWARE_IIC2_SDA_GPIO_PIN) :    \
                                                    GPIO_ResetBits(SOFTWARE_IIC2_SDA_GPIO_PORT, SOFTWARE_IIC2_SDA_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC2_READ_SDA()               GPIO_ReadInputDataBit(SOFTWARE_IIC2_SDA_GPIO_PORT, SOFTWARE_IIC2_SDA_GPIO_PIN)																								
																								

/* 函数错误代码 */
#define SOFTWARE_EOK      0   /* 没有错误 */
#define SOFTWARE_EID      1   /* ID错误 */
#define SOFTWARE_EACK     2   /* IIC通讯ACK错误 */
																								
/* 操作函数 */
void Software_iic_start(uint8_t type);                /* 产生IIC起始信号 */
void Software_iic_stop(uint8_t type);                 /* 产生IIC停止信号 */
uint8_t Software_iic_wait_ack(uint8_t type);          /* 等待IIC应答信号 */
void Software_iic_ack(uint8_t type);                  /* 产生ACK应答信号 */
void Software_iic_nack(uint8_t type);                 /* 不产生ACK应答信号 */
void Software_iic_send_byte(uint8_t type, uint8_t dat);     /* IIC发送一个字节 */
uint8_t Software_iic_read_byte(uint8_t type, uint8_t ack);  /* IIC接收一个字节 */
void Software_iic_init(uint8_t type);                 /* 初始化IIC接口 */

void Software_bitchange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data);
uint8_t Software_Read_Byte(uint8_t type,uint8_t ADDR,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t Software_Write_longByte(uint8_t type,uint8_t ADDR,uint8_t reg,uint8_t len,uint8_t *buf);

#endif /* __IIC_H__ */
