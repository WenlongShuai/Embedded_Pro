#include "hardware_iic.h"

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;

void Hardware_IIC_Init(uint8_t type)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;                                              //GPIO �ṹ�嶨��		
	I2C_InitTypeDef I2C_InitStructure;                                                //I2C �ṹ�嶨��

	switch(type)
	{
		case 0:   //IMU
		{
				//*I2C-IO ������*//
			IIC1_I2C_GPIO_APBxClock_FUN(IIC1_I2C_SCL_GPIO_CLK | IIC1_I2C_SDA_GPIO_CLK, ENABLE);                              //ʹ��GPIOBʱ��
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                   //���ù��ܵĿ�©���
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Pin = IIC1_I2C_SCL_PIN;                          
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                 //�ٶ����100MHz
			GPIO_Init(IIC1_I2C_SCL_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������ 
			
			GPIO_InitStructure.GPIO_Pin = IIC1_I2C_SDA_PIN;                                 //�ٶ����50MHz
			GPIO_Init(IIC1_I2C_SDA_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������ 
			
			GPIO_SetBits(IIC1_I2C_SCL_PORT, IIC1_I2C_SCL_PIN);
			GPIO_SetBits(IIC1_I2C_SDA_PORT, IIC1_I2C_SDA_PIN);
					
			//*I2C-ģʽ ����*//
			IIC1_I2C_APBxClock_FUN(IIC1_I2C_CLK,ENABLE);	                              //I2C2 ʱ��ʹ��		
			I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //ѡ��I2C����
			I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2CӦ��ʹ��
			I2C_InitStructure.I2C_ClockSpeed = 400000;                                        //ʱ�����ʣ��� HZ Ϊ��λ�ģ����Ϊ 400khz
			I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //�ò���ֻ���� I2C �����ڿ���ģʽ��ʱ�ӹ���Ƶ�ʸ��� 100KHz���²�������
			I2C_InitStructure.I2C_OwnAddress1 =0x0a;                                          //���õ�һ���豸�����ַ
			I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         //MPU6050 ��ַΪ 7 λ�������� 7 λ������
			I2C_Init(IIC1_I2Cx, &I2C_InitStructure);                                               //��ʼ���ṹ������
			I2C_Cmd(IIC1_I2Cx, ENABLE);               
			/* ���� I2C ȷ������ */
			I2C_AcknowledgeConfig(IIC1_I2Cx, ENABLE);
			
			I2C_GenerateSTOP(IIC1_I2Cx, ENABLE); 
			break;
		}
		case 1:   //OLED
		{
			//*I2C-IO ������*//
			IIC2_I2C_GPIO_APBxClock_FUN(IIC2_I2C_SCL_GPIO_CLK | IIC2_I2C_SCL_GPIO_CLK, ENABLE);                              //ʹ��GPIOBʱ��
			
			GPIO_PinAFConfig(IIC2_I2C_SDA_PORT, GPIO_PinSource0, GPIO_AF_I2C2);	
			GPIO_PinAFConfig(IIC2_I2C_SCL_PORT, GPIO_PinSource1, GPIO_AF_I2C2);	
			
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                   //���ù��ܵĿ�©���
			GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SCL_PIN;                          
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                 //�ٶ����100MHz
			GPIO_Init(IIC2_I2C_SCL_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������ 
			GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SDA_PIN;                        
			GPIO_Init(IIC2_I2C_SDA_PORT, &GPIO_InitStructure);                                            //��ʼ���ṹ������
			
			GPIO_SetBits(IIC2_I2C_SCL_PORT, IIC2_I2C_SCL_PIN);
			GPIO_SetBits(IIC2_I2C_SDA_PORT, IIC2_I2C_SDA_PIN);

			//*I2C-ģʽ ����*//
			IIC2_I2C_APBxClock_FUN(IIC2_I2C_CLK,ENABLE);	                              //I2C2 ʱ��ʹ��		
			I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //ѡ��I2C����
			I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2CӦ��ʹ��
			I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;                                        //ʱ�����ʣ��� HZ Ϊ��λ�ģ����Ϊ 400khz
			I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //�ò���ֻ���� I2C �����ڿ���ģʽ��ʱ�ӹ���Ƶ�ʸ��� 100KHz���²�������
			I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7;                                          //���õ�һ���豸�����ַ
			I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         //MPU6050 ��ַΪ 7 λ�������� 7 λ������
			I2C_Init(IIC2_I2Cx, &I2C_InitStructure);                                               //��ʼ���ṹ������
			I2C_Cmd(IIC2_I2Cx, ENABLE);               
			/* ���� I2C ȷ������ */
			I2C_AcknowledgeConfig(IIC2_I2Cx, ENABLE);
			
			I2C_GenerateSTOP(IIC2_I2Cx, ENABLE); 
			break;
		}
	}
}
	
	
//************IIC���ֽ�д����******************************************//	
uint8_t Hardware_Write_shortByte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t REG_data)
{   
	delay_ms(2); 	
	switch(type)
	{
		case 0:
		{
			I2C_GenerateSTART(IIC1_I2Cx, ENABLE);   
			I2CTimeout = I2CT_FLAG_TIMEOUT;	//������ʼλ

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}
				
			I2CTimeout = I2CT_FLAG_TIMEOUT;	
			I2C_Send7bitAddress(IIC1_I2Cx,(ADDR<<1) | 0,I2C_Direction_Transmitter);                        //����������ַ 
			while(!I2C_CheckEvent(IIC1_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
			
			I2C_SendData(IIC1_I2Cx,reg);                                                          //�Ĵ��������ַ
			
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
				
			I2C_SendData(IIC1_I2Cx, REG_data);                                                    //��������
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			
			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //���EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
				
			I2C_GenerateSTOP(IIC1_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
			break;
		}
		case 1:
		{
			I2C_GenerateSTART(IIC2_I2Cx, ENABLE);   
			I2CTimeout = I2CT_FLAG_TIMEOUT;	//������ʼλ

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}
				
			I2CTimeout = I2CT_FLAG_TIMEOUT;	
			I2C_Send7bitAddress(IIC2_I2Cx,(ADDR<<1) | 0,I2C_Direction_Transmitter);                        //����������ַ 
			while(!I2C_CheckEvent(IIC2_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
			
			I2C_SendData(IIC2_I2Cx,reg);                                                          //�Ĵ��������ַ
			
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
				
			I2C_SendData(IIC2_I2Cx, REG_data);                                                    //��������
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			
			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //���EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
				
			I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
			break;
		}
	}
	return 0;
}

//************IIC���ֽ�д����******************************************//	
uint8_t Hardware_Write_longByte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)
{   
	delay_ms(2); 
	switch(type)
	{
		case 0:
		{
			I2C_GenerateSTART(IIC1_I2Cx, ENABLE);                                                  //������ʼλ
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}

			I2C_Send7bitAddress(IIC1_I2Cx, (ADDR << 1) | 0, I2C_Direction_Transmitter);                       //����������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_SendData(IIC1_I2Cx, reg);                                                          //�Ĵ��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
			while(len--)                                                                      //���� while ѭ�� ��������
			{
				I2C_SendData(IIC1_I2Cx, *buf);                                                     //��������
				buf++;                                                                        //����ָ����λ
				I2CTimeout = I2CT_FLAG_TIMEOUT;
				while (!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //���EV8
				{
					if((I2CTimeout--) == 0) return 4;
				}
			}
			I2C_GenerateSTOP(IIC1_I2Cx, ENABLE);                                                   //����ֹͣ�ź�
			break;
		}
		case 1:
		{
			I2C_GenerateSTART(IIC2_I2Cx, ENABLE);                                                  //������ʼλ
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}
			
			I2C_Send7bitAddress(IIC2_I2Cx, (ADDR << 1) | 0, I2C_Direction_Transmitter);                       //����������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			
			while(!I2C_CheckEvent(IIC2_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_SendData(IIC2_I2Cx, reg);                                                          //�Ĵ��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			
			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
			while(len--)                                                                      //���� while ѭ�� ��������
			{
				I2C_SendData(IIC2_I2Cx, *buf);                                                     //��������
				buf++;                                                                        //����ָ����λ
				I2CTimeout = I2CT_FLAG_TIMEOUT;
				while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //���EV8
				{
					if((I2CTimeout--) == 0) return 4;
				}
			}
			I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);                                                   //����ֹͣ�ź�
			break;
		}
	}
	return 0;
}


//***********IIC���ֽڶ�����********************************************//
uint8_t Hardware_Read_Byte(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)                                //��Ҫ������ʼ�ź�
{  
	delay_ms(2);
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	
	switch(type)
	{
		case 0:
		{
			while(I2C_GetFlagStatus(IIC1_I2Cx, I2C_FLAG_BUSY))                                    //���ÿ⺯����� I2C �����Ƿ��� BUSY ״̬
			{
				if((I2CTimeout--) == 0) return 1;
			}
			I2C_GenerateSTART(IIC1_I2Cx, ENABLE);                                                  //�����ź�
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_Send7bitAddress(IIC1_I2Cx, (ADDR<<1)|0, I2C_Direction_Transmitter);                       //д��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //��� EV6
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
				
			I2C_SendData(IIC1_I2Cx, reg);                                                          //���Ͷ��ĵ�ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
				
			I2C_GenerateSTART(IIC1_I2Cx, ENABLE);                                                  //�����ź�
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 5;
			}
				
			I2C_Send7bitAddress(IIC1_I2Cx, (ADDR<<1)|1, I2C_Direction_Receiver);                          //��������ַ����������Ϊ��
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //���EV6
			{
				if((I2CTimeout--) == 0) return 6;
			}
				
			while(len)
			{
				if(len == 1)//ֻʣ�����һ������ʱ���� if ���
				{
					I2C_AcknowledgeConfig(IIC1_I2Cx, DISABLE);//�����һ������ʱ�ر�Ӧ��λ
					I2C_GenerateSTOP(IIC1_I2Cx, ENABLE);//���һ������ʱʹ��ֹͣλ
				}
				if(I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //��ȡ����
				{
					*buf = I2C_ReceiveData(IIC1_I2Cx);//���ÿ⺯��������ȡ���� pBuffer
					buf++; //ָ����λ
					len--;//�ֽ����� 1
					//delay_ms(1);
				}
			}				
			I2C_AcknowledgeConfig(IIC1_I2Cx, ENABLE);                                              //��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��	
			break;
		}
		case 1:
		{
			while(I2C_GetFlagStatus(IIC2_I2Cx, I2C_FLAG_BUSY))                                    //���ÿ⺯����� I2C �����Ƿ��� BUSY ״̬
			{
				if((I2CTimeout--) == 0) return 1;
			}
			I2C_GenerateSTART(IIC2_I2Cx, ENABLE);                                                  //�����ź�
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_Send7bitAddress(IIC2_I2Cx, (ADDR<<1)|0, I2C_Direction_Transmitter);                       //д��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //��� EV6
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
				
			I2C_SendData(IIC2_I2Cx, reg);                                                          //���Ͷ��ĵ�ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //��� EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
				
			I2C_GenerateSTART(IIC2_I2Cx, ENABLE);                                                  //�����ź�
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //��� EV5
			{
				if((I2CTimeout--) == 0) return 5;
			}
				
			I2C_Send7bitAddress(IIC2_I2Cx, (ADDR<<1)|1, I2C_Direction_Receiver);                          //��������ַ����������Ϊ��
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //���EV6
			{
				if((I2CTimeout--) == 0) return 6;
			}
				
			while(len)
			{
				if(len == 1)//ֻʣ�����һ������ʱ���� if ���
				{
					I2C_AcknowledgeConfig(IIC2_I2Cx, DISABLE);//�����һ������ʱ�ر�Ӧ��λ
					I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);//���һ������ʱʹ��ֹͣλ
				}
				if(I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //��ȡ����
				{
					*buf = I2C_ReceiveData(IIC2_I2Cx);//���ÿ⺯��������ȡ���� pBuffer
					buf++; //ָ����λ
					len--;//�ֽ����� 1
					//delay_ms(1);
				}
			}				
			I2C_AcknowledgeConfig(IIC2_I2Cx, ENABLE);                                              //��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��	
			break;
		}
	}
	return 0;				
}


//************IIC����д����******************************************//	
uint8_t Hardware_Write_commendByte(uint8_t type, uint8_t ADDR, uint8_t reg)
{   
	delay_ms(2);                                                                     //д�ֽڼ��
	
	switch(type)
	{
		case 0:
		{
			I2C_GenerateSTART(IIC1_I2Cx, ENABLE);                                                 //������ʼλ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}
				
			I2C_Send7bitAddress(IIC1_I2Cx,(ADDR<<1)|0,I2C_Direction_Transmitter);                        //����������ַ 
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_SendData(IIC1_I2Cx,reg);                                                          //�Ĵ��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC1_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
			I2C_GenerateSTOP(IIC1_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
			break;
		}
		case 1:
		{
			I2C_GenerateSTART(IIC2_I2Cx, ENABLE);                                                 //������ʼλ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //��� EV5
			{
				if((I2CTimeout--) == 0) return 1;
			}
				
			I2C_Send7bitAddress(IIC2_I2Cx,(ADDR<<1)|0,I2C_Direction_Transmitter);                        //����������ַ 
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1����� EV6
			{
				if((I2CTimeout--) == 0) return 2;
			}
				
			I2C_SendData(IIC2_I2Cx, reg);                                                          //�Ĵ��������ַ
			I2CTimeout = I2CT_FLAG_TIMEOUT;

			while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ����� EV8���������ݵ� DR �ȿ�������¼�
			{
				if((I2CTimeout--) == 0) return 3;
			}
				
			I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);                                                  //����ֹͣ�ź�
			break;
		}
	}
	return 0;
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
void Hardware_bitchange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	u8 buffer[1],b;
	switch(type)
	{
		case 0:
		{
			Hardware_Read_Byte(0,ADDR,reg,1,buffer);
			b=buffer[0];
			b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
			Hardware_Write_shortByte(0,ADDR, reg, b);
			break;
		}
		case 1:
		{
			Hardware_Read_Byte(1,ADDR,reg,1,buffer);
			b=buffer[0];
			b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
			Hardware_Write_shortByte(1,ADDR, reg, b);
			break;
		}
	}
}

/**************************IIC���ֽڶ�λ�ı�********************************************
*����ԭ��:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
����;  ADDR       ������ַ
       reg	      �Ĵ�����ַ
		   bitStart   Ŀ���ֽڵ���ʼλ
	     length     λ����
		   data       ��Ÿı�Ŀ���ֽ�λ��ֵ
		
����   �ɹ� Ϊ1 
 		   ʧ��Ϊ0
*******************************************************************************/ 
uint8_t Hardware_bitschange(uint8_t type, uint8_t ADDR, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{ 
	u8 buffer[1],b;
	switch(type)
	{
		case 0:
		{
			if (Hardware_Read_Byte(0,ADDR,reg,1,buffer) == 0) 
			{
				u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
				data <<= (8 - length);
				data >>= (7 - bitStart);
				b &= mask;
				b |= data;
				return Hardware_Write_shortByte(0,ADDR, reg, b);
			} 
			break;
		}
		case 1:
		{
			if (Hardware_Read_Byte(1,ADDR,reg,1,buffer) == 0) 
			{
				u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
				data <<= (8 - length);
				data >>= (7 - bitStart);
				b &= mask;
				b |= data;
				return Hardware_Write_shortByte(1,ADDR, reg, b);
			}			
			break;
		}
	}
	return 1;
}

