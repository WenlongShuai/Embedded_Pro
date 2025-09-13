#include "bsp_spi_flash.h"

/**
  * @brief  SPI 超时处理函数
  * @param  errorCode:错误代码编号
  * @retval 0
  */
static uint8_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  SPI_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  
  return 0;
}

/**
  * @brief  SPI GPIO初始化
  * @param  无
  * @retval 无
  */
static void SPI_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开时钟
	SPI_GPIO_RCC_ClockCmd(SPI_GPIO_CLK, ENABLE);
	
	//SCK
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = SPI_GPIO_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO_SCK_PORT, &GPIO_InitStruct);
	
	//MOSI
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = SPI_GPIO_MOSI_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO_MOSI_PORT, &GPIO_InitStruct);
	
	//MISO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = SPI_GPIO_MISO_PIN;
	GPIO_Init(SPI_GPIO_MISO_PORT, &GPIO_InitStruct);
	
	//CS
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //CS由软件设置，不使用硬件外设的CS
	GPIO_InitStruct.GPIO_Pin = SPI_GPIO_CS_PIN;
	GPIO_Init(SPI_GPIO_CS_PORT, &GPIO_InitStruct);
}

/**
  * @brief  SPI 外设配置初始化
  * @param  无
  * @retval 无
  */
static void SPI_Config()
{
	SPI_InitTypeDef SPI_InitStruct;
	
	SPIx_CLK_ClockCmd(SPIx_CLK, ENABLE);  //打开外设SPI时钟
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPIx_BAUDRATEPRE;
	SPI_InitStruct.SPI_CPHA = SPIx_CPHA;  //偶数边沿
	SPI_InitStruct.SPI_CPOL = SPIx_CPOL;  //SCK高电平空闲
	SPI_InitStruct.SPI_CRCPolynomial = SPIx_CRC;
	SPI_InitStruct.SPI_DataSize = SPIx_DATASIZE;
	SPI_InitStruct.SPI_Direction = SPIx_DIRECTION;
	SPI_InitStruct.SPI_FirstBit = SPIx_FIRSTBIT;
	SPI_InitStruct.SPI_Mode = SPIx_MODE;
	SPI_InitStruct.SPI_NSS = SPIx_NSS;
	
	SPI_Init(SPIx, &SPI_InitStruct);
	SPI_Cmd(SPIx, ENABLE);  //使能SPI
}

/**
  * @brief  SPI外设配置初始化以及GPIO初始化
  * @param  无
  * @retval 无
  */
void SPI_GPIO_Init()
{
	SPI_GPIO_Config();
	SPI_Config();
	SPI_FLASH_CS_HIGH();
}

/**
  * @brief  通过SPI总线发送数据到外部Flash中，每次发送一个字节大小的数据，如果想要每次发送2字节的大小的数据
						就配置SPI外设的SPIx_DATASIZE
	* @param  data:发送的数据
  * @retval flash返回的数据，因为SPI总线配置的是全双工，所以主机在发送数据的时候，主机也在接收从外部flash返回的数据
  */
uint16_t SPI_Flash_SendData(uint16_t data)
{
	uint16_t receiveData = 0;
	uint32_t SPITimeOut = SPIx_FLAG_TIMEOUT;
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeOut--) == 0)
		{
			return SPI_TIMEOUT_UserCallback(0);
		}
	}
	SPI_I2S_SendData(SPIx, data);
	SPITimeOut = SPIx_FLAG_TIMEOUT;
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeOut--) == 0)
		{
			return SPI_TIMEOUT_UserCallback(1);
		}	
	}
	receiveData = SPI_I2S_ReceiveData(SPIx);
	
	return receiveData;
}

/**
  * @brief  通过SPI总线接收外部Flash返回的数据
	* @param  无
  * @retval flash返回的数据
  */
uint16_t SPI_Flash_ReceiveData()
{
	return SPI_Flash_SendData(DUMMY_BYTE);
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，启动Flash的写使能
	* @param  无
  * @retval 无
  */
void SPI_Flash_WriteEnable()
{
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_WRITEENABLE);
	SPI_FLASH_CS_HIGH();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，等待外部Flash完成相应的工作，让flash处于空闲状态
	* @param  无
  * @retval 无
  */
void SPI_Flash_WaitForBUSY()
{
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_READSTATUSREG1);
	while((SPI_Flash_ReceiveData() & 0x01) == SET);
	SPI_FLASH_CS_HIGH();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，读取Flash的JEDECId
	* @param  无
  * @retval 返回Flash的JEDECId
  */
uint32_t SPI_Flash_Read_JEDECId()
{
	uint32_t JEDEC_Id = 0;
	SPI_FLASH_CS_LOW();
	
	SPI_Flash_SendData(FLASH_JEDECID);
	JEDEC_Id |= SPI_Flash_ReceiveData() << 16;
	JEDEC_Id |= SPI_Flash_ReceiveData() << 8;
	JEDEC_Id |= SPI_Flash_ReceiveData();
	SPI_FLASH_CS_HIGH();
	return JEDEC_Id;
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，读取Flash的DeviceId
	* @param  无
  * @retval 返回Flash的DeviceId
  */
uint8_t SPI_Flash_Read_ABHDeviceId()
{
	uint8_t deviceId = 0;
	SPI_FLASH_CS_LOW();
	
	SPI_Flash_SendData(ABH_DEVICEID);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	deviceId = SPI_Flash_ReceiveData();
	
	SPI_FLASH_CS_HIGH();
	return deviceId;
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，读取Flash的ManufacturerDeviceId
	* @param  无
  * @retval 返回Flash的ManufacturerDeviceId
  */
uint16_t SPI_Flash_Read_ManufacturerDeviceId()
{
	uint16_t ManufacturerDeviceId = 0;
	SPI_FLASH_CS_LOW();
	
	SPI_Flash_SendData(Manufacturer_DEVICEID);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(0x00);
	ManufacturerDeviceId |= SPI_Flash_ReceiveData() << 8;
	ManufacturerDeviceId |= SPI_Flash_ReceiveData();
	
	SPI_FLASH_CS_HIGH();
	return ManufacturerDeviceId;
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，读取Flash的UniqueDeviceId
	* @param  无
  * @retval 返回Flash的UniqueDeviceId(64位)
  */
uint64_t SPI_Flash_Read_UniqueDeviceId()
{
	uint64_t UniqueDeviceId = 0;
	uint8_t i = 0;
	SPI_FLASH_CS_LOW();
	
	SPI_Flash_SendData(Unique_DEVICEID);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	SPI_Flash_SendData(DUMMY_BYTE);
	
	for(i = 0;i < 8;i++)
	{
		UniqueDeviceId |= SPI_Flash_ReceiveData() << 8;
	}
	
	SPI_FLASH_CS_HIGH();
	return UniqueDeviceId;
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，向外部flash的状态寄存器写入数据
	* @param  无
  * @retval 无
  */
void SPI_Flash_WriteREG(uint16_t REGData)
{
	SPI_Flash_WriteEnable();
	SPI_Flash_WaitForBUSY();
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_WRITESTATUSREG);
	SPI_Flash_SendData((REGData & 0xFF00)>>8);
	SPI_Flash_SendData(REGData & 0xFF);
	SPI_FLASH_CS_HIGH();
	SPI_Flash_WaitForBUSY();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，读取readAddr地址中的一个字节数据
	* @param  readAddr:flash的内部地址
  * @retval 返回readAddr地址的数据
  */
uint8_t SPI_Flash_ReadByteData(uint32_t readAddr)
{
	uint8_t readData = 0;
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_READDATA);
	
	SPI_Flash_SendData((readAddr & 0xFF0000)>>16);
	SPI_Flash_SendData((readAddr & 0xFF00)>>8);
	SPI_Flash_SendData(readAddr & 0xFF);
	readData = SPI_Flash_ReceiveData();
	
	SPI_FLASH_CS_HIGH();
	
	return readData;
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，顺序读取readAddr地址上长度为readLen的数据
	* @param  readAddr:flash的内部地址
						readData:数据存放的地址
						readLen:读取数据的长度
  * @retval 无
  */
void SPI_Flash_SequenceReadData(uint32_t readAddr, uint8_t *readData, uint32_t readLen)
{
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_READDATA);
	
	SPI_Flash_SendData((readAddr & 0xFF0000)>>16);
	SPI_Flash_SendData((readAddr & 0xFF00)>>8);
	SPI_Flash_SendData(readAddr & 0xFF);
	
	while(readLen--)
	{
		*readData = SPI_Flash_ReceiveData();
		readData++;
	}
	
	SPI_FLASH_CS_HIGH();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，从pageAddr地址按页写入长度为pageLen的数据到flash中
	* @param  pageAddr:flash的内部地址
						pageData:写入的数据
						pageLen:写入数据的长度
  * @retval 无
  */
void SPI_Flash_PageWrite(uint32_t pageAddr, uint8_t *pageData, uint16_t pageLen)
{
	uint16_t len = pageLen;
	SPI_Flash_WriteEnable();
	SPI_Flash_WaitForBUSY();
	SPI_FLASH_CS_LOW();
	
	SPI_Flash_SendData(FLASH_PAGEPROGRAM);
	
	SPI_Flash_SendData((pageAddr & 0xFF0000) >> 16);
	SPI_Flash_SendData((pageAddr & 0xFF00) >> 8);
	SPI_Flash_SendData(pageAddr & 0xFF);
	
	if(pageLen > FLASH_PAGE_NUM)
	{
		len = FLASH_PAGE_NUM;
	}
	
	while(len--)
	{
		SPI_Flash_SendData(*pageData);
		pageData++;
	}

	SPI_FLASH_CS_HIGH();
	SPI_Flash_WaitForBUSY();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，从pageAddr地址顺序写入长度为writeLen的数据到flash中
	* @param  writeAddr:flash的内部地址
						writeData:写入的数据
						writeLen:写入数据的长度
  * @retval 无
  */
void SPI_Flash_SequenceWriteData(uint32_t writeAddr, uint8_t *writeData, uint32_t writeLen)
{
	uint8_t addr = writeAddr % FLASH_PAGE_NUM;
	uint16_t count = FLASH_PAGE_NUM - writeAddr;  //写入数据的地址到这页结束还剩多少个空位
	uint16_t flash_PageNum = writeLen / FLASH_PAGE_NUM;   //writeLen数据中有多少页
	uint16_t flash_singleNum = writeLen % FLASH_PAGE_NUM;  //writeLen数据中把页去掉，还剩多少个单独的数据
	uint16_t i = 0;
	if(addr == 0)   //地址对齐
	{
		for(i = 0;i < flash_PageNum;i++)
		{
			SPI_Flash_PageWrite(writeAddr+i*FLASH_PAGE_NUM, writeData+i*FLASH_PAGE_NUM, FLASH_PAGE_NUM);
		}
		SPI_Flash_PageWrite(writeAddr+i*FLASH_PAGE_NUM, writeData+i*FLASH_PAGE_NUM, flash_singleNum);
	}
	else   //地址未对齐
	{
		if(count >= writeLen)
		{
			SPI_Flash_PageWrite(writeAddr, writeData, writeLen);
		}
		else
		{
			SPI_Flash_PageWrite(writeAddr, writeData, count);
			writeAddr += count;
			writeData += count;
			
			flash_PageNum = (writeLen - count) / FLASH_PAGE_NUM;
			flash_singleNum = (writeLen - count) % FLASH_PAGE_NUM;
			i = 0;
			for(i = 0;i < flash_PageNum;i++)
			{
				SPI_Flash_PageWrite(writeAddr+i*FLASH_PAGE_NUM, writeData+i*FLASH_PAGE_NUM, FLASH_PAGE_NUM);
			}
			
			SPI_Flash_PageWrite(writeAddr+i*FLASH_PAGE_NUM, writeData+i*FLASH_PAGE_NUM, flash_singleNum);
		}
	}
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，从sectorEraseAddr地址扇区擦除内存空间，每次擦除4KB大小的内存
	* @param  sectorEraseAddr:擦除空间的地址
  * @retval 无
  */
void SPI_Flash_SectorErase(uint32_t sectorEraseAddr)
{
	SPI_Flash_WriteEnable();
	
	SPI_Flash_WaitForBUSY();
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_SECTORERASE);
	SPI_Flash_SendData((sectorEraseAddr & 0xFF0000) >> 16);
	SPI_Flash_SendData((sectorEraseAddr & 0xFF00) >> 8);
	SPI_Flash_SendData(sectorEraseAddr & 0xFF);
	
	SPI_FLASH_CS_HIGH();
	SPI_Flash_WaitForBUSY();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，从blockEraseAddr地址块擦除内存空间，每次擦除32KB/64KB大小的内存
	* @param  blockEraseAddr:擦除空间的地址
						flag:0:擦除32KB   1:擦除64KB
  * @retval 无
  */
void SPI_Flash_BlockErase_32K_64K(uint8_t flag, uint32_t blockEraseAddr)
{
	SPI_Flash_WriteEnable();
	SPI_Flash_WaitForBUSY();
	SPI_FLASH_CS_LOW();
	if(flag == BLOCK32K)
		SPI_Flash_SendData(FLASH_BLOCKERASE_32K);
	else
		SPI_Flash_SendData(FLASH_BLOCKERASE_64K);

	SPI_Flash_SendData((blockEraseAddr & 0xFF0000) >> 16);
	SPI_Flash_SendData((blockEraseAddr & 0xFF00) >> 8);
	SPI_Flash_SendData(blockEraseAddr & 0xFF);
	
	SPI_FLASH_CS_HIGH();
	SPI_Flash_WaitForBUSY();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，芯片擦除，直接擦除整个flash的空间
	* @param  无
  * @retval 无
  */
void SPI_Flash_ChipErase()
{
	SPI_Flash_WriteEnable();
	SPI_Flash_WaitForBUSY();
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_CHIPERASE);
	SPI_FLASH_CS_HIGH();
	SPI_Flash_WaitForBUSY();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，设置外部flash为下电模式
	* @param  无
  * @retval 无
  */
void SPI_Flash_PowerDown()
{
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_POWERDOWN);
	SPI_FLASH_CS_HIGH();
}

/**
  * @brief  主机通过SPI总线向外部flash发送指令，取消外部flash的下电模式
	* @param  无
  * @retval 无
  */
void SPI_Flash_ReleasePowerDown()
{
	SPI_FLASH_CS_LOW();
	SPI_Flash_SendData(FLASH_RELEASEPOWERDOWN);
	SPI_FLASH_CS_HIGH();
}
