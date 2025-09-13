/**
 ****************************************************************************************************
 * @file        atk_ms53l1m_iic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS53L0模块IIC接口驱动代码
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

#include "atk_ms53l1m_iic.h"
#include "delay.h"

/**
 * @brief       IIC接口延时函数，用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static inline void atk_ms53l1m_iic_delay(void)
{
    delay_us(4);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_iic_start(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
    
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_iic_stop(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SCL_MID(0);
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			break;
			#endif
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
}

/**
 * @brief       等待IIC应答信号
 * @param       无
 * @retval      0: 应答信号接收成功
 *              1: 应答信号接收失败
 */
uint8_t atk_ms53l1m_iic_wait_ack(uint8_t dir)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_LEFT())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_LEFT);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_MID())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_MID);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			
			while (ATK_MS53L1M_IIC_READ_SDA_RIGHT())
			{
					waittime++;
					
					if (waittime > 250)
					{
							atk_ms53l1m_iic_stop(DIR_RIGHT);
							rack = 1;
							break;
					}
			}
			
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
	return rack;
}

/**
 * @brief       产生ACK应答信号
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_iic_ack(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			#endif
			break;
	}
}

/**
 * @brief       不产生ACK应答信号
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_iic_nack(uint8_t dir)
{
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_LEFT(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			ATK_MS53L1M_IIC_SDA_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_MID(0);
			atk_ms53l1m_iic_delay();
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(1);
			atk_ms53l1m_iic_delay();
			ATK_MS53L1M_IIC_SCL_RIGHT(0);
			atk_ms53l1m_iic_delay();
		#endif
			break;
	}
}

/**
 * @brief       IIC发送一个字节
 * @param       dat: 要发送的数据
 * @retval      无
 */
void atk_ms53l1m_iic_send_byte(uint8_t dir, uint8_t dat)
{
    uint8_t t;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_LEFT((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_LEFT(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_LEFT(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_LEFT(1);
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_MID((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_MID(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_MID(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_MID(1);
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			for (t=0; t<8; t++)
			{
					ATK_MS53L1M_IIC_SDA_RIGHT((dat & 0x80) >> 7);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_RIGHT(1);
					atk_ms53l1m_iic_delay();
					ATK_MS53L1M_IIC_SCL_RIGHT(0);
					dat <<= 1;
			}
			ATK_MS53L1M_IIC_SDA_RIGHT(1);
			#endif
			break;
	}
}

/**
 * @brief       IIC接收一个字节
 * @param       ack: ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t atk_ms53l1m_iic_read_byte(uint8_t dir, uint8_t ack)
{
    uint8_t i;
    uint8_t dat = 0;
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_LEFT(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_LEFT())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_LEFT(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_LEFT);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_LEFT);
			}
			#endif
			break;
		case 2:
			#if ATK_MS53L1M_MID_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_MID(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_MID())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_MID(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_MID);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_MID);
			}
			#endif
			break;
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
			for (i = 0; i < 8; i++ )
			{
					dat <<= 1;
					ATK_MS53L1M_IIC_SCL_RIGHT(1);
					atk_ms53l1m_iic_delay();
					
					if (ATK_MS53L1M_IIC_READ_SDA_RIGHT())
					{
							dat++;
					}
					
					ATK_MS53L1M_IIC_SCL_RIGHT(0);
					atk_ms53l1m_iic_delay();
			}
			
			if (ack == 0)
			{
					atk_ms53l1m_iic_nack(DIR_RIGHT);
			}
			else
			{
					atk_ms53l1m_iic_ack(DIR_RIGHT);
			}
			#endif
			break;
	}
    
    return dat;
}

/**
 * @brief       初始化IIC接口
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_iic_init(uint8_t dir)
{
	GPIO_InitTypeDef gpio_init_struct = {0};
	
	switch(dir)
	{
		case 1:
			#if ATK_MS53L1M_LEFT_ON
			/* 使能SCL、SDA引脚GPIO的时钟 */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_LEFT();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_LEFT();
			
			/* 初始化SCL引脚 */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_LEFT; /* SCL引脚 */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* 推挽输出 */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* 高速 */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_LEFT, &gpio_init_struct);
			
			/* 初始化SDA引脚 */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_LEFT; /* SDA引脚 */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* 开漏输出 */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_LEFT, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_LEFT);
			#endif
			break;
		
		
		case 2:
			#if ATK_MS53L1M_MID_ON
	
			/* 使能SCL、SDA引脚GPIO的时钟 */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_MID();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_MID();
			
			/* 初始化SCL引脚 */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_MID; /* SCL引脚 */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* 推挽输出 */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* 高速 */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_MID, &gpio_init_struct);
			
			/* 初始化SDA引脚 */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_MID; /* SDA引脚 */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* 开漏输出 */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_MID, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_MID);
			#endif
			break;
		
		case 3:
			#if ATK_MS53L1M_RIGHT_ON
	
			/* 使能SCL、SDA引脚GPIO的时钟 */
			ATK_MS53L1M_IIC_SCL_GPIO_CLK_ENABLE_RIGHT();
			ATK_MS53L1M_IIC_SDA_GPIO_CLK_ENABLE_RIGHT();
			
			/* 初始化SCL引脚 */
			gpio_init_struct.GPIO_Pin    = ATK_MS53L1M_IIC_SCL_GPIO_PIN_RIGHT; /* SCL引脚 */
			gpio_init_struct.GPIO_Mode   = GPIO_Mode_OUT;          /* 推挽输出 */
			gpio_init_struct.GPIO_Speed  = GPIO_Speed_100MHz;         /* 高速 */
			gpio_init_struct.GPIO_OType = GPIO_OType_PP;
			gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(ATK_MS53L1M_IIC_SCL_GPIO_PORT_RIGHT, &gpio_init_struct);
			
			/* 初始化SDA引脚 */
			gpio_init_struct.GPIO_Pin = ATK_MS53L1M_IIC_SDA_GPIO_PIN_RIGHT; /* SDA引脚 */
			gpio_init_struct.GPIO_OType = GPIO_OType_OD;          /* 开漏输出 */
			GPIO_Init(ATK_MS53L1M_IIC_SDA_GPIO_PORT_RIGHT, &gpio_init_struct);
			
			atk_ms53l1m_iic_stop(DIR_RIGHT);
			#endif
			break;
	}
}
