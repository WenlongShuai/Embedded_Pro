/**
 ****************************************************************************************************
 * @file        atk_ms53l1m_iic.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-06-12
 * @brief       ATK-MS53L1Mģ��IIC�ӿ���������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MS53L1M_IIC_H
#define __ATK_MS53L1M_IIC_H

#include "./SYSTEM/sys/sys.h"

/* ���Ŷ��� */
#define ATK_MS53L1M_IIC_SCL_GPIO_PORT            GPIOB
#define ATK_MS53L1M_IIC_SCL_GPIO_PIN             GPIO_PIN_11
#define ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define ATK_MS53L1M_IIC_SDA_GPIO_PORT            GPIOB
#define ATK_MS53L1M_IIC_SDA_GPIO_PIN             GPIO_PIN_10
#define ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/* IO���� */
#define ATK_MS53L1M_IIC_SCL(x)                   do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(ATK_MS53L1M_IIC_SCL_GPIO_PORT, ATK_MS53L1M_IIC_SCL_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(ATK_MS53L1M_IIC_SCL_GPIO_PORT, ATK_MS53L1M_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_SDA(x)                   do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(ATK_MS53L1M_IIC_SDA_GPIO_PORT, ATK_MS53L1M_IIC_SDA_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(ATK_MS53L1M_IIC_SDA_GPIO_PORT, ATK_MS53L1M_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)

#define ATK_MS53L1M_IIC_READ_SDA()               HAL_GPIO_ReadPin(ATK_MS53L1M_IIC_SDA_GPIO_PORT, ATK_MS53L1M_IIC_SDA_GPIO_PIN)

/* �������� */
void atk_ms53l1m_iic_start(void);                /* ����IIC��ʼ�ź� */
void atk_ms53l1m_iic_stop(void);                 /* ����IICֹͣ�ź� */
uint8_t atk_ms53l1m_iic_wait_ack(void);          /* �ȴ�IICӦ���ź� */
void atk_ms53l1m_iic_ack(void);                  /* ����ACKӦ���ź� */
void atk_ms53l1m_iic_nack(void);                 /* ������ACKӦ���ź� */
void atk_ms53l1m_iic_send_byte(uint8_t dat);     /* IIC����һ���ֽ� */
uint8_t atk_ms53l1m_iic_read_byte(uint8_t ack);  /* IIC����һ���ֽ� */
void atk_ms53l1m_iic_init(void);                 /* ��ʼ��IIC�ӿ� */

#endif
