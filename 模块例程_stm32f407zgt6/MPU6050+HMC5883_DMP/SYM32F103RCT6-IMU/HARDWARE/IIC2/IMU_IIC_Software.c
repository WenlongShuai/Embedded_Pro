/**
 ****************************************************************************************************
 * @file        atk_ms6050_iic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050模块IIC接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "IMU_IIC_Software.h"
#include "delay.h"

/**
 * @brief       IIC接口延时函数，用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static __INLINE void Software_iic_delay(void)
{
	delay_us(2);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void Software_iic_start(void)
{
	SOFTWARE_IIC_SDA(1);
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void Software_iic_stop(void)
{
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
}

/**
 * @brief       等待IIC应答信号
 * @param       无
 * @retval      0: 应答信号接收成功
 *              1: 应答信号接收失败
 */
uint8_t Software_iic_wait_ack(void)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;

	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();

	while (SOFTWARE_IIC_READ_SDA())
	{
		waittime++;
		if (waittime > 250)
		{
			Software_iic_stop();
			rack = 1;
			break;
		}
	}

	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();

	return rack;
}

/**
 * @brief       产生ACK应答信号
 * @param       无
 * @retval      无
 */
void Software_iic_ack(void)
{
	SOFTWARE_IIC_SDA(0);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
}

/**
 * @brief       不产生ACK应答信号
 * @param       无
 * @retval      无
 */
void Software_iic_nack(void)
{
	SOFTWARE_IIC_SDA(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(1);
	Software_iic_delay();
	SOFTWARE_IIC_SCL(0);
	Software_iic_delay();
}

/**
 * @brief       IIC发送一个字节
 * @param       dat: 要发送的数据
 * @retval      无
 */
void Software_iic_send_byte(uint8_t dat)
{
	uint8_t t;

	for (t=0; t<8; t++)
	{
		SOFTWARE_IIC_SDA((dat & 0x80) >> 7);
		Software_iic_delay();
		SOFTWARE_IIC_SCL(1);
		Software_iic_delay();
		SOFTWARE_IIC_SCL(0);
		dat <<= 1;
	}
	SOFTWARE_IIC_SDA(1);
}

/**
 * @brief       IIC接收一个字节
 * @param       ack: ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t Software_iic_read_byte(uint8_t ack)
{
	uint8_t i;
	uint8_t dat = 0;

	for (i = 0; i < 8; i++ )
	{
		dat <<= 1;
		SOFTWARE_IIC_SCL(1);
		Software_iic_delay();

		if (SOFTWARE_IIC_READ_SDA())
		{
				dat++;
		}

		SOFTWARE_IIC_SCL(0);
		Software_iic_delay();
	}

	if (ack == 0)
	{
		Software_iic_nack();
	}
	else
	{
		Software_iic_ack();
	}

	return dat;
}


u8 Software_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|1);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	while (len)
	{
		*buf = Software_iic_read_byte((len > 1) ? 1 : 0);
		len--;
		buf++;
	}
	Software_iic_stop();
	return SOFTWARE_EOK;
}
	
	
u8 Software_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{
	uint8_t i;
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
		Software_iic_stop();
		return SOFTWARE_EACK;
	}
	for (i=0; i<len; i++)
	{
		Software_iic_send_byte(buf[i]);
		if (Software_iic_wait_ack() == 1)
		{
			Software_iic_stop();
			return SOFTWARE_EACK;
		}
	}
	Software_iic_stop();
	return SOFTWARE_EOK;

}

	
/**************************IIC单字节单位改变********************************************
*函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*功　　能:	     读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入：	ADDR     器件地址
      reg	     寄存器地址
		  bitNum   要修改目标字节的bitNum位
		  data     为0 时，目标位将被清0 否则将被置位
		   
返回   成功为1  
 		   失败为0 
*******************************************************************************/ 
void Software_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data)
{
	u8 buffer[1],b;
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|1);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	*buffer = Software_iic_read_byte(0);
	Software_iic_stop();
		
	b=buffer[0];
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	
	Software_iic_start();
	Software_iic_send_byte((ADDR<<1)|0);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(reg);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_send_byte(b);
	if (Software_iic_wait_ack() == 1)
	{
			Software_iic_stop();
			return;
	}
	Software_iic_stop();
}

/**
 * @brief       初始化IIC接口
 * @param       无
 * @retval      无
 */
void Software_iic_init(void)
{
	GPIO_InitTypeDef gpio_init_struct;

	/* 使能SCL、SDA引脚GPIO的时钟 */
	SOFTWARE_IIC_SCL_GPIO_CLK_ENABLE();
	SOFTWARE_IIC_SDA_GPIO_CLK_ENABLE();

	/* 初始化SCL引脚 */
	gpio_init_struct.GPIO_Pin = SOFTWARE_IIC_SCL_GPIO_PIN;  /* SCL引脚 */
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_OD;          /* 推挽输出 */
	gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                  /* 上拉 */
	GPIO_Init(SOFTWARE_IIC_SCL_GPIO_PORT, &gpio_init_struct);
	/* 初始化SDA引脚 */
	gpio_init_struct.GPIO_Pin = SOFTWARE_IIC_SDA_GPIO_PIN;  /* SDA引脚 */
	GPIO_Init(SOFTWARE_IIC_SDA_GPIO_PORT, &gpio_init_struct);

	Software_iic_stop();
}
