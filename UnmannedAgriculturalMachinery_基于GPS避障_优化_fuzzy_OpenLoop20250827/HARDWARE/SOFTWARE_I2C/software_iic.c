#include "software_iic.h"
#include "delay.h"


//��ʼ��IIC
void Software_iic_init(uint8_t type)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	switch(type)
	{
		case 0:   //IMU
		{
			RCC_AHB1PeriphClockCmd(SOFTWARE_IIC1_SCL_GPIO_RCC | SOFTWARE_IIC1_SDA_GPIO_RCC, ENABLE);//ʹ��GPIOʱ��
			//GPIOB8,B9��ʼ������
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC1_SCL_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
			GPIO_Init(SOFTWARE_IIC1_SCL_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
			
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC1_SDA_GPIO_PIN;
			GPIO_Init(SOFTWARE_IIC1_SDA_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
			SOFTWARE_IIC1_SCL(1);
			SOFTWARE_IIC1_SDA(1);
			break;
		}
		case 1:		//OLED
		{
			RCC_AHB1PeriphClockCmd(SOFTWARE_IIC2_SCL_GPIO_RCC, ENABLE);//ʹ��GPIOʱ��
			
			//GPIOB8,B9��ʼ������
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC2_SCL_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
			GPIO_Init(SOFTWARE_IIC2_SCL_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
			
			GPIO_InitStructure.GPIO_Pin = SOFTWARE_IIC2_SDA_GPIO_PIN;
			GPIO_Init(SOFTWARE_IIC2_SDA_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
			
			SOFTWARE_IIC2_SCL(1);
			SOFTWARE_IIC2_SDA(1);
			break;
		}
	}
}

/**
 * @brief       IIC�ӿ���ʱ���������ڿ���IIC��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static __INLINE void Software_iic_delay(void)
{
	delay_us(4);
}

/**
 * @brief       ����IIC��ʼ�ź�
 * @param       ��
 * @retval      ��
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
 * @brief       ����IICֹͣ�ź�
 * @param       ��
 * @retval      ��
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
 * @brief       �ȴ�IICӦ���ź�
 * @param       ��
 * @retval      0: Ӧ���źŽ��ճɹ�
 *              1: Ӧ���źŽ���ʧ��
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
 * @brief       ����ACKӦ���ź�
 * @param       ��
 * @retval      ��
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
 * @brief       ������ACKӦ���ź�
 * @param       ��
 * @retval      ��
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
 * @brief       IIC����һ���ֽ�
 * @param       dat: Ҫ���͵�����
 * @retval      ��
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
 * @brief       IIC����һ���ֽ�
 * @param       ack: ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
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

	
/**************************IIC���ֽڵ�λ�ı�********************************************
*����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*��������:	     �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
���룺	ADDR     ������ַ
      reg	     �Ĵ�����ַ
		  bitNum   Ҫ�޸�Ŀ���ֽڵ�bitNumλ
		  data     Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
		   
����   �ɹ�Ϊ1  
 		   ʧ��Ϊ0 
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

