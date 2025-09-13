#include "hardware_iic.h"

static __IO uint32_t  I2CTimeout = HARDWARE_IICx_LONG_TIMEOUT;

void Hardware_IIC_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;                                              //GPIO 结构体定义		
	I2C_InitTypeDef I2C_InitStructure;                                                //I2C 结构体定义

	//*I2C-IO 口配置*//
	HARDWARE_IICx_GPIO_APBxClock_FUN(HARDWARE_IICx_SCL_GPIO_CLK | HARDWARE_IICx_SDA_GPIO_CLK, ENABLE);                              //使能GPIOB时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                                   //复用功能的开漏输出
	GPIO_InitStructure.GPIO_Pin = HARDWARE_IICx_SCL_PIN;                          
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;                                 //速度最高100MHz
	GPIO_Init(HARDWARE_IICx_SCL_PORT, &GPIO_InitStructure);                                            //初始化结构体配置 
	
	GPIO_InitStructure.GPIO_Pin = HARDWARE_IICx_SDA_PIN;                                 //速度最高50MHz
	GPIO_Init(HARDWARE_IICx_SDA_PORT, &GPIO_InitStructure);                                            //初始化结构体配置 
			
	//*I2C-模式 配置*//
	HARDWARE_IICx_APBxClock_FUN(HARDWARE_IICx_CLK,ENABLE);	                              //I2C2 时钟使能		
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //选择I2C功能
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2C应答使能
	I2C_InitStructure.I2C_ClockSpeed = HARDWARE_IICx_Speed;                                        //时钟速率，以 HZ 为单位的，最高为 400khz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //该参数只有在 I2C 工作在快速模式（时钟工作频率高于 100KHz）下才有意义
	I2C_InitStructure.I2C_OwnAddress1 = HARDWARE_IICx_OWN_ADDRESS7;                                          //设置第一个设备自身地址
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         
	I2C_Init(HARDWARE_IICx_I2Cx, &I2C_InitStructure);                                               //初始化结构体配置
	I2C_Cmd(HARDWARE_IICx_I2Cx, ENABLE);               
}
	
	
//************IIC单字节写函数******************************************//	
uint8_t Hardware_Write_shortByte(uint8_t ADDR, uint8_t reg, uint8_t REG_data)
{
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);   
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;	//产生起始位

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //清除 EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}
		
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;	
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx,ADDR,I2C_Direction_Transmitter);                        //发送器件地址 
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1，清除 EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx,reg);                                                          //寄存器具体地址
	
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, REG_data);                                                    //发送数据
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //清除EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                  //产生停止信号
	
	return 0;
}

//************IIC多字节写函数******************************************//	
uint8_t Hardware_Write_longByte(uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)
{   
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //产生起始位
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}

	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                       //发送器件地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1，清除 EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx, reg);                                                          //寄存器具体地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
	while(len--)                                                                      //利用 while 循环 发送数据
	{
		I2C_SendData(HARDWARE_IICx_I2Cx, *buf);                                                     //发送数据
		buf++;                                                                        //数据指针移位
		I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
		while (!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //清除EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
	}
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                   //产生停止信号

	return 0;
}

uint8_t Hardware_Write_nbyte(uint8_t dev_address, uint16_t address, uint8_t len, uint8_t *wbuf)
{		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //产生起始位
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}

	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address, I2C_Direction_Transmitter);                       //发送器件地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1，清除 EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx, address >> 8);                                                        
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
	{
		if((I2CTimeout--) == 0) return 3;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx, address & 0x00FF);                                                        
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	while(len--)                                                                      //利用 while 循环 发送数据
	{
		I2C_SendData(HARDWARE_IICx_I2Cx, *wbuf);                                                     //发送数据
		wbuf++;                                                                        //数据指针移位
		I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
		while (!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //清除EV8
		{
			if((I2CTimeout--) == 0) return 5;
		}
	}
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                   //产生停止信号

	return 0;
		
		
}

//***********IIC多字节读函数********************************************//
uint8_t Hardware_Read_Byte(uint8_t ADDR, uint8_t reg, uint8_t len, uint8_t *buf)                                //需要两个起始信号
{  
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(HARDWARE_IICx_I2Cx, I2C_FLAG_BUSY))                                    //调用库函数检测 I2C 器件是否处于 BUSY 状态
	{
		if((I2CTimeout--) == 0) return 1;
	}
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //开启信号
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                       //写入器件地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //清除 EV6
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, reg);                                                          //发送读的地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //清除 EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //开启信号
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 5;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR|1, I2C_Direction_Receiver);                          //将器件地址传出，主机为读
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //清除EV6
	{
		if((I2CTimeout--) == 0) return 6;
	}
		
	while(len)
	{
		if(len == 1)//只剩下最后一个数据时进入 if 语句
		{
			I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, DISABLE);//最后有一个数据时关闭应答位
			I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);//最后一个数据时使能停止位
		}
		if(I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //读取数据
		{
			*buf = I2C_ReceiveData(HARDWARE_IICx_I2Cx);//调用库函数将数据取出到 pBuffer
			buf++; //指针移位
			len--;//字节数减 1
			//delay_ms(1);
		}
	}				
	I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, ENABLE);                                              //将应答位使能回去，等待下次通信	
			
	return 0;				
}

uint8_t Hardware_Read_nbyte(uint8_t dev_address, uint16_t data, uint8_t len, uint8_t *rbuf)
{
	delay_ms(2);
	
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;
	
	while(I2C_GetFlagStatus(HARDWARE_IICx_I2Cx, I2C_FLAG_BUSY))                                    //调用库函数检测 I2C 器件是否处于 BUSY 状态
	{
		if((I2CTimeout--) == 0) return 1;
	}
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //开启信号
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address, I2C_Direction_Transmitter);                       //写入器件地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //清除 EV6
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
		
	I2C_SendData(HARDWARE_IICx_I2Cx, data >> 8);                                                     
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //清除 EV8
	{
		if((I2CTimeout--) == 0) return 4;
	}
	
	I2C_SendData(HARDWARE_IICx_I2Cx, data & 0x00FF);                                                     
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //清除 EV8
	{
		if((I2CTimeout--) == 0) return 5;
	}
		
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                  //开启信号
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
	{
		if((I2CTimeout--) == 0) return 6;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, dev_address|1, I2C_Direction_Receiver);                          //将器件地址传出，主机为读
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //清除EV6
	{
		if((I2CTimeout--) == 0) return 7;
	}
		
	while(len)
	{
		if(len == 1)//只剩下最后一个数据时进入 if 语句
		{
			I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, DISABLE);//最后有一个数据时关闭应答位
			I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);//最后一个数据时使能停止位
		}
		if(I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //读取数据
		{
			*rbuf = I2C_ReceiveData(HARDWARE_IICx_I2Cx);//调用库函数将数据取出到 pBuffer
			rbuf++; //指针移位
			len--;//字节数减 1
			//delay_ms(1);
		}
	}				
	I2C_AcknowledgeConfig(HARDWARE_IICx_I2Cx, ENABLE);                                              //将应答位使能回去，等待下次通信	
			
	return 0;				

}

//************IIC单独写命令******************************************//	
uint8_t Hardware_Write_commendByte(uint8_t ADDR, uint8_t reg)
{   
	I2C_GenerateSTART(HARDWARE_IICx_I2Cx, ENABLE);                                                 //产生起始位
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))                      //清除 EV5
	{
		if((I2CTimeout--) == 0) return 1;
	}
		
	I2C_Send7bitAddress(HARDWARE_IICx_I2Cx, ADDR, I2C_Direction_Transmitter);                        //发送器件地址 
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1，清除 EV6
	{
		if((I2CTimeout--) == 0) return 2;
	}
		
	I2C_SendData(HARDWARE_IICx_I2Cx,reg);                                                          //寄存器具体地址
	I2CTimeout = HARDWARE_IICx_FLAG_TIMEOUT;

	while(!I2C_CheckEvent(HARDWARE_IICx_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
	{
		if((I2CTimeout--) == 0) return 3;
	}
		
	I2C_GenerateSTOP(HARDWARE_IICx_I2Cx, ENABLE);                                                  //产生停止信号
			
	return 0;
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
void Hardware_bitchange(uint8_t ADDR, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	u8 buffer[1],b;

	Hardware_Read_Byte(ADDR,reg,1,buffer);
	b=buffer[0];
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	Hardware_Write_shortByte(ADDR, reg, b);
}

/**************************IIC单字节多位改变********************************************
*函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入;  ADDR       器件地址
       reg	      寄存器地址
		   bitStart   目标字节的起始位
	     length     位长度
		   data       存放改变目标字节位的值
		
返回   成功 为1 
 		   失败为0
*******************************************************************************/ 
uint8_t Hardware_bitschange(uint8_t ADDR, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{ 
	u8 buffer[1],b;

	if (Hardware_Read_Byte(ADDR, reg, 1, buffer) == 0) 
	{
		u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return Hardware_Write_shortByte(ADDR, reg, b);
	} 
			
	return 1;
}

