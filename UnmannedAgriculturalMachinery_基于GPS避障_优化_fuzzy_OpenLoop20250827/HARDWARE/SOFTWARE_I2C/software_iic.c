#include "software_iic.h"
#include "delay.h"


//初始化IIC
void Software_iic_init(uint8_t type)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	switch(type)
	{
		case 0:   //IMU
		{
			RCC_AHB1PeriphClockCmd(SOFTWARE_IIC1_SCL_GPIO_RCC | SOFTWARE_IIC1_SDA_GPIO_RCC, ENABLE);//使能GPIO时钟
			//GPIOB8,B9初始化设置
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC1_SCL_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
			GPIO_Init(SOFTWARE_IIC1_SCL_GPIO_PORT, &GPIO_InitStructure);//初始化
			
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC1_SDA_GPIO_PIN;
			GPIO_Init(SOFTWARE_IIC1_SDA_GPIO_PORT, &GPIO_InitStructure);//初始化
			SOFTWARE_IIC1_SCL(1);
			SOFTWARE_IIC1_SDA(1);
			break;
		}
		case 1:		//OLED
		{
			RCC_AHB1PeriphClockCmd(SOFTWARE_IIC2_SCL_GPIO_RCC, ENABLE);//使能GPIO时钟
			
			//GPIOB8,B9初始化设置
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC2_SCL_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
			GPIO_Init(SOFTWARE_IIC2_SCL_GPIO_PORT, &GPIO_InitStructure);//初始化
			
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC2_SDA_GPIO_PIN;
			GPIO_Init(SOFTWARE_IIC2_SDA_GPIO_PORT, &GPIO_InitStructure);//初始化
			
			SOFTWARE_IIC2_SCL(1);
			SOFTWARE_IIC2_SDA(1);
			break;
		}
	}
}

/**
 * @brief       IIC接口延时函数，用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static __INLINE void Software_iic_delay(void)
{
	delay_us(4);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void Software_iic_start(uint8_t type)
{
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_OUT();
			SOFTWARE_IIC1_SDA(1);
			SOFTWARE_IIC1_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(0);
			Software_iic_delay();
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_OUT();
			SOFTWARE_IIC2_SDA(1);
			SOFTWARE_IIC2_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(0);
			Software_iic_delay();
			break;
		}
	}
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void Software_iic_stop(uint8_t type)
{
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_OUT();
			SOFTWARE_IIC1_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SDA(1);
			Software_iic_delay();
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_OUT();
			SOFTWARE_IIC2_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SDA(1);
			Software_iic_delay();
			break;
		}
	}
}

/**
 * @brief       等待IIC应答信号
 * @param       无
 * @retval      0: 应答信号接收成功
 *              1: 应答信号接收失败
 */
uint8_t Software_iic_wait_ack(uint8_t type)
{
	uint8_t waittime = 0;
	uint8_t rack = 0;
	
	switch(type)
	{
		case 0:
		{			
			SOFTWARE_IIC1_SDA_IN();
			SOFTWARE_IIC1_SDA(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(1);
			Software_iic_delay();

			while (SOFTWARE_IIC1_READ_SDA())
			{
				waittime++;
				if (waittime > 250)
				{
					Software_iic_stop(0);  //IMU
					rack = 1;
					break;
				}
			}

			SOFTWARE_IIC1_SCL(0);
			Software_iic_delay();
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_IN();
			SOFTWARE_IIC2_SDA(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(1);
			Software_iic_delay();

			while (SOFTWARE_IIC2_READ_SDA())
			{
				waittime++;
				if (waittime > 250)
				{
					Software_iic_stop(1);  //OLED
					rack = 1;
					break;
				}
			}

			SOFTWARE_IIC2_SCL(0);
			Software_iic_delay();
			break;
		}
	}
	return rack;
}

/**
 * @brief       产生ACK应答信号
 * @param       无
 * @retval      无
 */
void Software_iic_ack(uint8_t type)
{
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_OUT();
			SOFTWARE_IIC1_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(0);
			Software_iic_delay();
			SOFTWARE_IIC1_SDA(1);
			Software_iic_delay();
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_OUT();
			SOFTWARE_IIC2_SDA(0);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(0);
			Software_iic_delay();
			SOFTWARE_IIC2_SDA(1);
			Software_iic_delay();
			break;
		}
	}
}

/**
 * @brief       不产生ACK应答信号
 * @param       无
 * @retval      无
 */
void Software_iic_nack(uint8_t type)
{
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_OUT();
			SOFTWARE_IIC1_SDA(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC1_SCL(0);
			Software_iic_delay();
			break;
		}
		
		case 1:
		{
			SOFTWARE_IIC2_SDA_OUT();
			SOFTWARE_IIC2_SDA(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(1);
			Software_iic_delay();
			SOFTWARE_IIC2_SCL(0);
			Software_iic_delay();
			break;
		}
	}
}

/**
 * @brief       IIC发送一个字节
 * @param       dat: 要发送的数据
 * @retval      无
 */
void Software_iic_send_byte(uint8_t type, uint8_t dat)
{
	uint8_t t;
	
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_OUT();
			for (t=0; t<8; t++)
			{
				SOFTWARE_IIC1_SDA((dat & 0x80) >> 7);
				Software_iic_delay();
				SOFTWARE_IIC1_SCL(1);
				Software_iic_delay();
				SOFTWARE_IIC1_SCL(0);
				dat <<= 1;
			}
			SOFTWARE_IIC1_SDA(1);
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_OUT();
			for (t=0; t<8; t++)
			{
				SOFTWARE_IIC2_SDA((dat & 0x80) >> 7);
				Software_iic_delay();
				SOFTWARE_IIC2_SCL(1);
				Software_iic_delay();
				SOFTWARE_IIC2_SCL(0);
				dat <<= 1;
			}
			SOFTWARE_IIC2_SDA(1);
			break;
		}
	}
}

/**
 * @brief       IIC接收一个字节
 * @param       ack: ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t Software_iic_read_byte(uint8_t type, uint8_t ack)
{
	uint8_t i;
	uint8_t dat = 0;
	
	switch(type)
	{
		case 0:
		{
			SOFTWARE_IIC1_SDA_IN();
			for (i = 0; i < 8; i++ )
			{
				dat <<= 1;
				SOFTWARE_IIC1_SCL(1);
				Software_iic_delay();

				if (SOFTWARE_IIC1_READ_SDA())
				{
						dat++;
				}

				SOFTWARE_IIC1_SCL(0);
				Software_iic_delay();
			}

			if (ack == 0)
			{
				Software_iic_nack(0);  //IMU
			}
			else
			{
				Software_iic_ack(0);  //IMU
			}
			break;
		}
		case 1:
		{
			SOFTWARE_IIC2_SDA_IN();
			for (i = 0; i < 8; i++ )
			{
				dat <<= 1;
				SOFTWARE_IIC2_SCL(1);
				Software_iic_delay();

				if (SOFTWARE_IIC2_READ_SDA())
				{
						dat++;
				}

				SOFTWARE_IIC2_SCL(0);
				Software_iic_delay();
			}

			if (ack == 0)
			{
				Software_iic_nack(1);  //OLED
			}
			else
			{
				Software_iic_ack(1);  //OLED
			}
			break;
		}
	}
	return dat;
}


uint8_t Software_Read_Byte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)
{
	switch(type)
	{
		case 0:
		{
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|0);
			if (Software_iic_wait_ack(0) == 1)
			{
				Software_iic_stop(0);
				return SOFTWARE_EACK;
			}
			Software_iic_send_byte(0,reg);
			if (Software_iic_wait_ack(0) == 1)
			{
				Software_iic_stop(0);
				return SOFTWARE_EACK;
			}
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|1);
			if (Software_iic_wait_ack(0) == 1)
			{
				Software_iic_stop(0);
				return SOFTWARE_EACK;
			}
			while (len)
			{
				*buf = Software_iic_read_byte(0,(len > 1) ? 1 : 0);
				len--;
				buf++;
			}
			Software_iic_stop(0);
			break;
		}
		case 1:
		{
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|0);
			if (Software_iic_wait_ack(1) == 1)
			{
				Software_iic_stop(1);
				return SOFTWARE_EACK;
			}
			Software_iic_send_byte(1,reg);
			if (Software_iic_wait_ack(1) == 1)
			{
				Software_iic_stop(1);
				return SOFTWARE_EACK;
			}
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|1);
			if (Software_iic_wait_ack(1) == 1)
			{
				Software_iic_stop(1);
				return SOFTWARE_EACK;
			}
			while (len)
			{
				*buf = Software_iic_read_byte(1,(len > 1) ? 1 : 0);
				len--;
				buf++;
			}
			Software_iic_stop(1);
			break;
		}
	}
	
	return SOFTWARE_EOK;
}
	
	
uint8_t Software_Write_longByte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	switch(type)
	{
		case 0:
		{
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|0);
			if (Software_iic_wait_ack(0) == 1)
			{
				Software_iic_stop(0);
				return SOFTWARE_EACK;
			}
			Software_iic_send_byte(0,reg);
			if (Software_iic_wait_ack(0) == 1)
			{
				Software_iic_stop(0);
				return SOFTWARE_EACK;
			}
			for (i=0; i<len; i++)
			{
				Software_iic_send_byte(0,buf[i]);
				if (Software_iic_wait_ack(0) == 1)
				{
					Software_iic_stop(0);
					return SOFTWARE_EACK;
				}
			}
			Software_iic_stop(0);
			break;
		}
		case 1:
		{
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|0);
			if (Software_iic_wait_ack(1) == 1)
			{
				Software_iic_stop(1);
				return SOFTWARE_EACK;
			}
			Software_iic_send_byte(1,reg);
			if (Software_iic_wait_ack(1) == 1)
			{
				Software_iic_stop(1);
				return SOFTWARE_EACK;
			}
			for (i=0; i<len; i++)
			{
				Software_iic_send_byte(1,buf[i]);
				if (Software_iic_wait_ack(1) == 1)
				{
					Software_iic_stop(1);
					return SOFTWARE_EACK;
				}
			}
			Software_iic_stop(1);
			break;
		}
	}
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
void Software_bitchange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	uint8_t buffer[1],b;
	switch(type)
	{
		case 0:
		{
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|0);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			Software_iic_send_byte(0,reg);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|1);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			*buffer = Software_iic_read_byte(0,0);
			Software_iic_stop(0);
				
			b=buffer[0];
			b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
			
			Software_iic_start(0);
			Software_iic_send_byte(0,(ADDR<<1)|0);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			Software_iic_send_byte(0,reg);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			Software_iic_send_byte(0,b);
			if (Software_iic_wait_ack(0) == 1)
			{
					Software_iic_stop(0);
					return;
			}
			Software_iic_stop(0);
			break;
		}
		case 1:
		{
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|0);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			Software_iic_send_byte(1,reg);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|1);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			*buffer = Software_iic_read_byte(1,0);
			Software_iic_stop(1);
				
			b=buffer[0];
			b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
			
			Software_iic_start(1);
			Software_iic_send_byte(1,(ADDR<<1)|0);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			Software_iic_send_byte(1,reg);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			Software_iic_send_byte(1,b);
			if (Software_iic_wait_ack(1) == 1)
			{
					Software_iic_stop(1);
					return;
			}
			Software_iic_stop(1);
			break;
		}
	}
}

