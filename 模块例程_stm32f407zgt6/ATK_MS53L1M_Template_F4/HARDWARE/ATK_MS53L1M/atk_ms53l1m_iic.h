/**
 ****************************************************************************************************
 * @file        atk_ms53l1m_iic.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS53L1M模块IIC接口驱动代码
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

#ifndef __ATK_MS53L1M_IIC_H
#define __ATK_MS53L1M_IIC_H

#include "sys.h"

#define DIR_LEFT    1
#define DIR_MID			2
#define DIR_RIGHT		3

#define ATK_MS53L1M_LEFT_ON 1
#define ATK_MS53L1M_MID_ON 1
#define ATK_MS53L1M_RIGHT_ON 1

/*******左侧激光*******/																								
#if ATK_MS53L1M_LEFT_ON
/* 引脚定义 */
#define ATK_MS53L1M_IIC_SCL_GPIO_PORT_LEFT            GPIOF
#define ATK_MS53L1M_IIC_SCL_GPIO_PIN_LEFT             GPIO_Pin_2
#define ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_LEFT()    do{ RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); }while(0)
#define ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT            GPIOF
#define ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT             GPIO_Pin_3
#define ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_LEFT()    do{ RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); }while(0)

/* IO操作 */
#define ATK_MS53L1M_IIC_SCL_LEFT(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_LEFT, ATK_MS53L1M_IIC_SCL_GPIO_PIN_LEFT) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_LEFT, ATK_MS53L1M_IIC_SCL_GPIO_PIN_LEFT);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_SDA_LEFT(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_READ_SDA_LEFT()               GPIO_ReadInputDataBit(ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT)

#endif

/*******中间激光*******/																								
#if ATK_MS53L1M_MID_ON
/* 引脚定义 */
#define ATK_MS53L1M_IIC_SCL_GPIO_PORT_MID            GPIOB
#define ATK_MS53L1M_IIC_SCL_GPIO_PIN_MID             GPIO_Pin_11
#define ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_MID()    do{ RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); }while(0)
#define ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID            GPIOB
#define ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID             GPIO_Pin_10
#define ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_MID()    do{ RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); }while(0)

/* IO操作 */
#define ATK_MS53L1M_IIC_SCL_MID(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_MID, ATK_MS53L1M_IIC_SCL_GPIO_PIN_MID) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_MID, ATK_MS53L1M_IIC_SCL_GPIO_PIN_MID);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_SDA_MID(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID, ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID, ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_READ_SDA_MID()               GPIO_ReadInputDataBit(ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID, ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID)

#endif

/*******右侧激光*******/																								
#if ATK_MS53L1M_RIGHT_ON
/* 引脚定义 */
#define ATK_MS53L1M_IIC_SCL_GPIO_PORT_RIGHT            GPIOF
#define ATK_MS53L1M_IIC_SCL_GPIO_PIN_RIGHT             GPIO_Pin_11
#define ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_RIGHT()    do{ RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); }while(0)
#define ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT            GPIOF
#define ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT             GPIO_Pin_12
#define ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_RIGHT()    do{ RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); }while(0)

/* IO操作 */
#define ATK_MS53L1M_IIC_SCL_RIGHT(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_RIGHT, ATK_MS53L1M_IIC_SCL_GPIO_PIN_RIGHT) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SCL_GPIO_PORT_RIGHT, ATK_MS53L1M_IIC_SCL_GPIO_PIN_RIGHT);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_SDA_RIGHT(x)                   do{ x ?                                                                                             \
                                                    GPIO_SetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT) :    \
                                                    GPIO_ResetBits(ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_READ_SDA_RIGHT()               GPIO_ReadInputDataBit(ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT, ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT)

#endif																								
																								
																								
																								
																								
/* 操作函数 */
void atk_ms53l1m_iic_start(uint8_t dir);                /* 产生IIC起始信号 */
void atk_ms53l1m_iic_stop(uint8_t dir);                 /* 产生IIC停止信号 */
uint8_t atk_ms53l1m_iic_wait_ack(uint8_t dir);          /* 等待IIC应答信号 */
void atk_ms53l1m_iic_ack(uint8_t dir);                  /* 产生ACK应答信号 */
void atk_ms53l1m_iic_nack(uint8_t dir);                 /* 不产生ACK应答信号 */
void atk_ms53l1m_iic_send_byte(uint8_t dir, uint8_t dat);     /* IIC发送一个字节 */
uint8_t atk_ms53l1m_iic_read_byte(uint8_t dir, uint8_t ack);  /* IIC接收一个字节 */
void atk_ms53l1m_iic_init(uint8_t dir);                 /* 初始化IIC接口 */

#endif
