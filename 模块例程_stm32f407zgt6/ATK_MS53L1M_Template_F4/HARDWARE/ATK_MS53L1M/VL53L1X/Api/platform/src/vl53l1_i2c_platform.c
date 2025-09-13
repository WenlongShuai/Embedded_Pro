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

#include "delay.h"
//#include "hardware_iic.h"
#include "atk_ms53l1m_iic.h"
#include "vl53l1_platform_log.h"

uint8_t VL53l1X_read_data(uint8_t dir, uint8_t dev_address, uint8_t address, uint8_t *data)
{
	uint8_t temp = 0;
	#if 1
	
	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address);
	atk_ms53l1m_iic_wait_ack(dir);

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address | 1);
	atk_ms53l1m_iic_wait_ack(dir);
	temp = atk_ms53l1m_iic_read_byte(dir, 0);
	atk_ms53l1m_iic_stop(dir);

	#else
	uint8_t ret = 0;
	ret = Hardware_Read_Byte(dev_address, address, 1, &temp);
	printf("VL53l1X_read_data ret = %d\r\n",ret);
	#endif

	*data = temp;
	
	return temp;
}

void VL53l1X_read_nbyte(uint8_t dir, uint8_t dev_address, uint8_t address, uint8_t len, uint8_t *rbuf)
{
	#if 1
	int i = 0;

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address);
	atk_ms53l1m_iic_wait_ack(dir);

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address | 1);
	atk_ms53l1m_iic_wait_ack(dir);

	for (i = 0; i < len; i++)
	{
			if (i == len - 1)
			{
					rbuf[i] = atk_ms53l1m_iic_read_byte(dir, 0);
			}
			else
			{
					rbuf[i] = atk_ms53l1m_iic_read_byte(dir, 1);
			}
	}

	atk_ms53l1m_iic_stop(dir);
			
	#else
	uint8_t ret = 0;
	ret = Hardware_Read_Byte(dev_address, address, len, rbuf);
	printf("VL53l1X_read_nbyte ret = %d\r\n",ret);
	#endif
}

void VL53l1X_read_byte(uint8_t dir, uint8_t dev_address, uint16_t data, uint8_t len, uint8_t *rbuf)
{
	#if 1
	int i = 0;

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, data >> 8);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, data & 0x00FF);
	atk_ms53l1m_iic_wait_ack(dir);

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address | 1);
	atk_ms53l1m_iic_wait_ack(dir);

	for (i = 0; i < len; i++)
	{
			if (i == len - 1)
			{
					rbuf[i] = atk_ms53l1m_iic_read_byte(dir, 0);
			}
			else
					rbuf[i] = atk_ms53l1m_iic_read_byte(dir, 1);
	}

	atk_ms53l1m_iic_stop(dir);

	#else
	uint8_t ret = 0;
	ret = Hardware_Read_nbyte(dev_address, data, len, rbuf);
	printf("VL53l1X_read_byte ret = %d\r\n",ret);
	#endif
}

void VL53l1X_write_data(uint8_t dir, uint8_t dev_address, uint8_t address, uint8_t data)
{
	#if 1

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, data);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_stop(dir);

	#else
	uint8_t ret = 0;
	ret = Hardware_Write_shortByte(dev_address, address, data);
	printf("VL53l1X_write_data ret = %d\r\n",ret);
	#endif
}

void VL53l1X_write_byte(uint8_t dir, uint8_t dev_address, uint8_t address, uint8_t len, uint8_t *wbuf)
{
	#if 1
	int i = 0;

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address);
	atk_ms53l1m_iic_wait_ack(dir);

	for (i = 0; i < len; i++)
	{
		atk_ms53l1m_iic_send_byte(dir, wbuf[i]);
		atk_ms53l1m_iic_wait_ack(dir);
	}

	atk_ms53l1m_iic_stop(dir);
			
	#else
	uint8_t ret = 0;
	ret = Hardware_Write_longByte(dev_address, address, len, wbuf);
	printf("VL53l1X_write_byte ret = %d\r\n",ret);
	#endif
}

void VL53l1X_write_nbyte(uint8_t dir, uint8_t dev_address, uint16_t address, uint8_t len, uint8_t *wbuf)
{
	#if 1
	int i = 0;

	atk_ms53l1m_iic_start(dir);
	atk_ms53l1m_iic_send_byte(dir, dev_address);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address >> 8);
	atk_ms53l1m_iic_wait_ack(dir);
	atk_ms53l1m_iic_send_byte(dir, address & 0x00FF);
	atk_ms53l1m_iic_wait_ack(dir);

	for (i = 0; i < len; i++)
	{
		atk_ms53l1m_iic_send_byte(dir, wbuf[i]);
		atk_ms53l1m_iic_wait_ack(dir);
	}

	atk_ms53l1m_iic_stop(dir);
	
	#else
	uint8_t ret = 0;
	ret = Hardware_Write_nbyte(dev_address, address, len, wbuf);
	printf("VL53l1X_write_nbyte ret = %d\r\n",ret);
	#endif
}
