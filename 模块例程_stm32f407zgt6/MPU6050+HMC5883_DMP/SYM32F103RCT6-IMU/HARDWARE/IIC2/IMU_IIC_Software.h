/**
 ****************************************************************************************************
 * @file        atk_ms6050_iic.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050模块IIC接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __IMU_IIC_SOFTWARE_H
#define __IMU_IIC_SOFTWARE_H

#include "sys.h"

/* 引脚定义 */
#define SOFTWARE_IIC_SCL_GPIO_PORT            GPIOB
#define SOFTWARE_IIC_SCL_GPIO_PIN             GPIO_Pin_10
#define SOFTWARE_IIC_SCL_GPIO_CLK_ENABLE()    do{ RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }while(0)
#define SOFTWARE_IIC_SDA_GPIO_PORT            GPIOB
#define SOFTWARE_IIC_SDA_GPIO_PIN             GPIO_Pin_11
#define SOFTWARE_IIC_SDA_GPIO_CLK_ENABLE()    do{ RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }while(0)

/* IO操作 */
#define SOFTWARE_IIC_SCL(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC_SCL_GPIO_PORT, SOFTWARE_IIC_SCL_GPIO_PIN):    \
                                                    GPIO_ResetBits(SOFTWARE_IIC_SCL_GPIO_PORT, SOFTWARE_IIC_SCL_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC_SDA(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN) :    \
                                                    GPIO_ResetBits(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN);   \
                                                }while(0)

#define SOFTWARE_IIC_READ_SDA()               GPIO_ReadInputDataBit(SOFTWARE_IIC_SDA_GPIO_PORT, SOFTWARE_IIC_SDA_GPIO_PIN)

																								
																								/* 函数错误代码 */
#define SOFTWARE_EOK      0   /* 没有错误 */
#define SOFTWARE_EID      1   /* ID错误 */
#define SOFTWARE_EACK     2   /* IIC通讯ACK错误 */
																								
/* 操作函数 */
void Software_iic_start(void);                /* 产生IIC起始信号 */
void Software_iic_stop(void);                 /* 产生IIC停止信号 */
uint8_t Software_iic_wait_ack(void);          /* 等待IIC应答信号 */
void Software_iic_ack(void);                  /* 产生ACK应答信号 */
void Software_iic_nack(void);                 /* 不产生ACK应答信号 */
void Software_iic_send_byte(uint8_t dat);     /* IIC发送一个字节 */
uint8_t Software_iic_read_byte(uint8_t ack);  /* IIC接收一个字节 */
void Software_iic_init(void);                 /* 初始化IIC接口 */

void Software_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data);
u8 Software_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf);
u8 Software_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf);

#endif /* __IMU_IIC_SOFTWARE_H */
