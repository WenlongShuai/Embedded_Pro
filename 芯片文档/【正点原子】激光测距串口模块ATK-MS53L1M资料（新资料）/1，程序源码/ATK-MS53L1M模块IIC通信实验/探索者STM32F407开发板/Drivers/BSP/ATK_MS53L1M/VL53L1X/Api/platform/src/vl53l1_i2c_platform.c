/*
 * COPYRIGHT (C) STMicroelectronics 2015. All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * STMicroelectronics ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with STMicroelectronics
 *
 * Programming Golden Rule: Keep it Simple!
 *
 */

/*!
 * \file   VL53L0X_platform.c
 * \brief  Code function defintions for Ewok Platform Layer
 *
 */

#include <stdio.h>    // sprintf(), vsnprintf(), printf()

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include "vl53l1_i2c_platform.h"
#include "vl53l1_def.h"

#include "./SYSTEM/delay/delay.h"
#include "./BSP/ATK_MS53L1M/atk_ms53l1m_iic.h"
#include "vl53l1_platform_log.h"

uint8_t VL53l1X_read_data(uint8_t dev_address, uint8_t address, uint8_t *data)
{
    uint8_t temp = 0;
    
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address);
    atk_ms53l1m_iic_wait_ack();

    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address | 1);
    atk_ms53l1m_iic_wait_ack();
    temp = atk_ms53l1m_iic_read_byte(0);
    atk_ms53l1m_iic_stop();
    *data = temp;
    
    return temp;
}

void VL53l1X_read_nbyte(uint8_t dev_address, uint8_t address, uint8_t len, uint8_t *rbuf)
{
    int i = 0;
    
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address);
    atk_ms53l1m_iic_wait_ack();

    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address | 1);
    atk_ms53l1m_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            rbuf[i] = atk_ms53l1m_iic_read_byte(0);
        }
        else
        {
            rbuf[i] = atk_ms53l1m_iic_read_byte(1);
        }
    }

    atk_ms53l1m_iic_stop();
}

void VL53l1X_read_byte(uint8_t dev_address, uint16_t data, uint8_t len, uint8_t *rbuf)
{
    int i = 0;
    
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(data >> 8);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(data & 0x00FF);
    atk_ms53l1m_iic_wait_ack();

    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address | 1);
    atk_ms53l1m_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            rbuf[i] = atk_ms53l1m_iic_read_byte(0);
        }
        else
            rbuf[i] = atk_ms53l1m_iic_read_byte(1);
    }

    atk_ms53l1m_iic_stop();
}

void VL53l1X_write_data(uint8_t dev_address, uint8_t address, uint8_t data)
{
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(data);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_stop();
}

void VL53l1X_write_byte(uint8_t dev_address, uint8_t address, uint8_t len, uint8_t *wbuf)
{
    int i = 0;
    
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address);
    atk_ms53l1m_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        atk_ms53l1m_iic_send_byte(wbuf[i]);
        atk_ms53l1m_iic_wait_ack();
    }

    atk_ms53l1m_iic_stop();
}

void VL53l1X_write_nbyte(uint8_t dev_address, uint16_t address, uint8_t len, uint8_t *wbuf)
{
    int i = 0;
    
    atk_ms53l1m_iic_start();
    atk_ms53l1m_iic_send_byte(dev_address);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address >> 8);
    atk_ms53l1m_iic_wait_ack();
    atk_ms53l1m_iic_send_byte(address & 0x00FF);
    atk_ms53l1m_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        atk_ms53l1m_iic_send_byte(wbuf[i]);
        atk_ms53l1m_iic_wait_ack();
    }

    atk_ms53l1m_iic_stop();
}
