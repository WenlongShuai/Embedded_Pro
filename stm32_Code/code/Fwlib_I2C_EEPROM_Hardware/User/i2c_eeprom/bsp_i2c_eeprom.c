#include "bsp_i2c_eeprom.h"

/**
  * @brief  I2C 超时处理函数
  * @param  errorCode:错误代码编号
  * @retval 0
  */
static  uint8_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  EEPROM_ERROR("I2C 等待超时!errorCode = %d",errorCode);
  
  return 0;
}

/**
  * @brief  I2C GPIO的配置及初始化
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开PB6、PB7的时钟
	RCC_APB2PeriphClockCmd(I2C_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = I2C_GPIO_SCL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO_SCL_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = I2C_GPIO_SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO_SDA_PORT, &GPIO_InitStruct);
}

/**
  * @brief  I2C 外设相关的配置及初始化
  * @param  无
  * @retval 无
  */
static void I2C_Config()
{
	I2C_InitTypeDef I2C_InitStruct;
	//打开I2C外设的时钟
	RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
	
	I2C_InitStruct.I2C_OwnAddress1 = I2C_OWNADDRESS1;
	I2C_InitStruct.I2C_Mode = I2C_MODE;
	I2C_InitStruct.I2C_DutyCycle = I2C_DUTYCYCLE;
	I2C_InitStruct.I2C_ClockSpeed = I2C_CLOCKSPEED;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_ACKNOWLEDGEDADDRESS;
	I2C_InitStruct.I2C_Ack = I2C_ACK;
	
	I2C_Init(DEBUG_I2C, &I2C_InitStruct);
	
	I2C_Cmd(DEBUG_I2C, ENABLE);
}

/**
  * @brief  I2C 所有的初始化
  * @param  无
  * @retval 无
  */
void I2C_EEPROM_Init()
{
	I2C_GPIO_Config();
	I2C_Config();
}

/**
  * @brief  通过I2C把存储器的一个字节数据写入到EEPROM中
	* @param  writeAddr:写入EEPROM的数据地址
						data:写入EEPROM的数据
  * @retval 无
  */
void I2C_EEPROM_WriteByte(uint8_t writeAddr, uint8_t data)
{
	uint32_t I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET );  //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)  //检测EV5事件(主要检测start信号是否发送完成)
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(0);
			return;
		}
	} 
	
	I2C_SendData(DEBUG_I2C, 0xA0);  //主机发送从机设备地址+写方向
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测设备地址+写位是否发送成功，以及数据寄存器是否为空 */
//	while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
//	while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
//	while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)  //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(1);
			return;
		}
	}
	
	I2C_SendData(DEBUG_I2C, writeAddr);    //主机向从机发送写入数据的地址
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);  //检测数据寄存器是否为空
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)  //检测EV8事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(2);
			return;
		}
	}
	
	I2C_SendData(DEBUG_I2C, data);  //主机向从机发送写入的数据
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测数据寄存器是否为空，检测字节发送是否结束 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_BTF) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR) //检测EV8_2事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(3);
			return;
		}
	}
	
	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
}

/**
  * @brief  I2C把存储器的数据写入EEPROM中（按页写入），一页最多能写8byte
	* @param  writeAddr:页写入数据时EEPROM的首地址
						data:写入EEPROM的数据
						dataLen:写入EEPROM数据的大小
  * @retval 无
  */
void I2C_EEPROM_WritePage(uint8_t writeAddr, uint8_t *dataArray, uint8_t dataLen)
{
	uint8_t i = 0;
	uint32_t I2CTimeOut = I2Cx_LONG_TIMEOUT;
	if(dataLen > EEPROM_BYTE_ALIG)  //AT24C02按页写入最大字节数为8byte
	{
		return;
	} 
	
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) //检测EV5事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(4);
			return;
		}
	}

	I2C_SendData(DEBUG_I2C, 0xA0);  //主机发送从机设备地址+写方向
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	/* 检测设备地址+写位是否发送成功，以及数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)  //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(5);
			return;
		}
	}
	
		
	I2C_SendData(DEBUG_I2C, writeAddr);    //主机向从机发送写入数据的地址
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);   //检测数据寄存器是否为空
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)  //检测EV8事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(6);
			return;
		}
	}

	while(dataLen--)
	{
		I2C_SendData(DEBUG_I2C, dataArray[i]);  //主机向从机发送写入的数据
		I2CTimeOut = I2Cx_LONG_TIMEOUT;
		/* 检测数据寄存器是否为空，检测字节发送是否结束 */
		//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
		//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_BTF) == RESET);
		while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR) //检测EV8事件
		{
			if((I2CTimeOut--) == 0)
			{
				I2C_TIMEOUT_UserCallback(7);
				return;
			}
		}
		
		i++;
	}
	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
}

/**
  * @brief  I2C把存储器的数据写入EEPROM中，EEPROM最多能存储256byte的数据
	* @param  writeAddr:页写入数据时EEPROM的首地址
						data:写入EEPROM的数据
						dataLen:写入EEPROM数据的大小
  * @retval 无
  */
void I2C_EEPROM_WriteData(uint8_t writeAddr, uint8_t *data, uint16_t dataLen)
{
	uint8_t addr = writeAddr % EEPROM_BYTE_ALIG;   //判断写入的地址是否为地址对齐
	uint8_t writePageNum = dataLen / EEPROM_BYTE_ALIG;  //求出dataLen有多少个Page
	uint8_t writeSingleNum = dataLen % EEPROM_BYTE_ALIG;  //求出dataLen写完Page之后，还剩多少个单独的字节数据(凑不满一页)
	
	uint8_t i = 0;
	
	if(addr == 0)  //写入的地址字节对齐
	{
		for(i = 0;i < writePageNum;i++)
		{
			I2C_EEPROM_WritePage(writeAddr+i*EEPROM_BYTE_ALIG, data+i*EEPROM_BYTE_ALIG, EEPROM_BYTE_ALIG);  //每次写入一页的数据
			EEPROM_WaitForWriteEnd();  //写完之后，防止EEPROM内部的时钟还在进行写操作，不然后面向EEPROM发送信号时，EEPROM收不到信号，这里等待EEPROM内部写完
		}
		I2C_EEPROM_WritePage(writeAddr+i*EEPROM_BYTE_ALIG, data+i*EEPROM_BYTE_ALIG, writeSingleNum); //写入剩下的字节数据
		EEPROM_WaitForWriteEnd(); //写完之后，防止EEPROM内部的时钟还在进行写操作，不然后面向EEPROM发送信号时，EEPROM收不到信号，这里等待EEPROM内部写完
	}
	else    //写入的地址字节没对齐
	{
		uint8_t writeAddrPagePos = writeAddr / 8;  //求出writeAddr在EEPROM中的哪一个page
		uint8_t writeAddrEmptyPosNum = EEPROM_BYTE_ALIG * (1 + writeAddrPagePos) - writeAddr; //求出writeAddr所在的page中还剩多少个字节
		I2C_EEPROM_WritePage(writeAddr, data, writeAddrEmptyPosNum);  //把writeAddr所在页剩余的空间写满
		EEPROM_WaitForWriteEnd(); //写完之后，防止EEPROM内部的时钟还在进行写操作，不然后面向EEPROM发送信号时，EEPROM收不到信号，这里等待EEPROM内部写完
		
		writePageNum = (dataLen - writeAddrEmptyPosNum) / EEPROM_BYTE_ALIG;  //求出写完writeAddr所在页剩余的空间之后剩余的数据可以写满多少Page
		for(i = 0;i < writePageNum;i++)
		{
			I2C_EEPROM_WritePage(writeAddr+writeAddrEmptyPosNum+i*EEPROM_BYTE_ALIG, data+writeAddrEmptyPosNum+i*EEPROM_BYTE_ALIG, EEPROM_BYTE_ALIG);//每次写入一页的数据
			EEPROM_WaitForWriteEnd();//写完之后，防止EEPROM内部的时钟还在进行写操作，不然后面向EEPROM发送信号时，EEPROM收不到信号，这里等待EEPROM内部写完
		}
		
		writeSingleNum = dataLen - writePageNum * EEPROM_BYTE_ALIG - writeAddrEmptyPosNum;  //求出最后剩下多少数据没写
		I2C_EEPROM_WritePage(writeAddr+writeAddrEmptyPosNum+i*EEPROM_BYTE_ALIG, data+writeAddrEmptyPosNum+i*EEPROM_BYTE_ALIG, writeSingleNum);  //写入最后几个数据
		EEPROM_WaitForWriteEnd();//写完之后，防止EEPROM内部的时钟还在进行写操作，不然后面向EEPROM发送信号时，EEPROM收不到信号，这里等待EEPROM内部写完
	}
}


/**
  * @brief  I2C读取EEPROM当前地址（地址计数器+1）中的数据（1byte）
	* @param  无
  * @retval 返回读取的数据
  */
uint8_t I2C_EEPROM_ReadCurrent()
{
	uint32_t I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	uint8_t readData = 0;
	
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)  //检测EV5事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(8);
		}
	}

	I2C_SendData(DEBUG_I2C, 0xA1);  //主机发送从机设备地址+读方向
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测设备地址+读位是否发送成功，以及数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)  //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(9);
		}
	}
	
	
	I2C_AcknowledgeConfig(DEBUG_I2C, DISABLE);  //主机产生非应答信号
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测发送/接收寄存器是否接收到数据，检测数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == SET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_RXNE) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR) //检测EV7事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(10);
		}
	}
	
	readData = I2C_ReceiveData(DEBUG_I2C);
	
	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
	
	I2C_AcknowledgeConfig(DEBUG_I2C, ENABLE);  //主机产生应答信号

	return readData;
}

/**
  * @brief  I2C随机读取EEPROM指定地址中的数据（1byte）
	* @param  readAddr:读取EEPROM中哪个地址的数据
  * @retval 返回指定地址的数据
  */
uint8_t I2C_EEPROM_ReadRandom(uint8_t readAddr)
{
	uint8_t readData = 0;
	uint32_t I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)  //检测EV5事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(11);
		}
	}

	I2C_SendData(DEBUG_I2C, 0xA0);  //主机发送从机设备地址+写方向
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测设备地址+读位是否发送成功，以及数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)  //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(12);
		}
	}

	
	I2C_SendData(DEBUG_I2C, readAddr); //主机发送需要从 从机 哪个地址读取数据
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);   //检测数据寄存器是否为空
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR) //检测EV8事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(13);
		}
	}
	
	
	/* 在读操作时，第二次发送起始信号 */
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(14);
		}
	}

	I2C_SendData(DEBUG_I2C, 0xA1);  //主机发送从机设备地址+读方向
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET); //在读操作时，只需要检测地址是否被发送
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)  //检测接收方向的EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(15);
		}
	}

	I2C_AcknowledgeConfig(DEBUG_I2C, DISABLE);  //主机产生非应答信号
	I2CTimeOut = I2Cx_FLAG_TIMEOUT;
	/* 检测发送/接收寄存器是否接收到数据，检测数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == SET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_RXNE) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR) //检测EV7事件
	{
		if((I2CTimeOut--) == 0)
		{
			return I2C_TIMEOUT_UserCallback(16);
		}
	}
	
	readData = I2C_ReceiveData(DEBUG_I2C);
	
	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
	I2C_AcknowledgeConfig(DEBUG_I2C, ENABLE);
	
	return readData;
}

/**
  * @brief  I2C顺序读取EEPROM的数据
	* @param  readAddr:从EEPROM中哪个地址开始读取数据
						readData:数据存放的存储器地址
						readLen:顺序读取数据的大小
  * @retval 无
  */
void I2C_EEPROM_ReadSequential(uint8_t readAddr,uint8_t *readData , uint16_t readLen)
{
	uint8_t i = 0;
	uint32_t I2CTimeOut = I2Cx_LONG_TIMEOUT;
	
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) //检测EV5事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(17);
			return;
		}
	}

	
	I2C_SendData(DEBUG_I2C, 0xA0);  //主机发送从机设备地址+写方向
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	/* 检测设备地址+读位是否发送成功，以及数据寄存器是否为空 */
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == RESET);
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(18);
			return;
		}
	}

	
	I2C_SendData(DEBUG_I2C, readAddr); //主机发送需要从 从机 哪个地址读取数据
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TXE) == RESET);  //检测数据寄存器是否为空
 	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)  //检测EV8事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(19);
			return;
		}
	}


	/* 在读操作时，第二次发送起始信号 */
	I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	//while( I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET ); //检测start信号是否发送成功
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) //检测EV5事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(20);
			return;
		}
	}


	I2C_SendData(DEBUG_I2C, 0xA1);  //主机发送从机设备地址+读方向
	I2CTimeOut = I2Cx_LONG_TIMEOUT;
	//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);  //在读操作时，只需要检测地址是否被发送
	while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)  //检测EV6事件
	{
		if((I2CTimeOut--) == 0)
		{
			I2C_TIMEOUT_UserCallback(21);
			return;
		}
	}

	
	while(readLen)
	{
		if(readLen == 1)
		{
			I2C_AcknowledgeConfig(DEBUG_I2C, DISABLE);  //主机向从机发送应答信号
		}
		I2CTimeOut = I2Cx_LONG_TIMEOUT;
		/* 检测发送/接收寄存器是否接收到数据，检测数据寄存器是否为空 */
		//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_TRA) == SET);
		//while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_RXNE) == RESET);
		while(I2C_CheckEvent(DEBUG_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)  //检测EV7事件
		{
			if((I2CTimeOut--) == 0)
			{
				I2C_TIMEOUT_UserCallback(22);
				return;
			}
		}

		readData[i] = I2C_ReceiveData(DEBUG_I2C);
		
		i++;
		readLen--;
	}

	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
	I2C_AcknowledgeConfig(DEBUG_I2C, ENABLE);  //主机向从机发送应答信号
}

/**
  * @brief  I2C在进行读写操作完后，等待EEPROM内部时序读写完成
	* @param  无
  * @retval 无
  */
void EEPROM_WaitForWriteEnd()
{	
	uint32_t I2CTimeOut1 = I2C_FLAG_TIMEOUT;
	uint32_t I2CTimeOut2 = I2C_FLAG_TIMEOUT;
	do
	{
		I2C_GenerateSTART(DEBUG_I2C, ENABLE);  //主机产生start信号
		I2CTimeOut1 = I2C_FLAG_TIMEOUT;
		while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_SB) == RESET) //检测EV5事件
		{
			if((I2CTimeOut1--) == 0)
			{
				I2C_TIMEOUT_UserCallback(23);
				return;
			}
		}
		I2C_SendData(DEBUG_I2C, 0xA0);  //主机发送从机设备地址+写方向
		
		if((I2CTimeOut2--) == 0)
		{
			I2C_TIMEOUT_UserCallback(24);
			return;
		}
	}while(I2C_GetFlagStatus(DEBUG_I2C, I2C_FLAG_ADDR) == RESET);
	I2C_GenerateSTOP(DEBUG_I2C, ENABLE);  //主机产生stop信号
}

