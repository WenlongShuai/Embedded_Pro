#include "W25Qxx.h"
#include "key.h"
#include "FreeRTOS.h"
#include "semphr.h"  // 信号量相关类型和函数的声明

POSI_UNION posi_now = {0};

// 
extern SemaphoreHandle_t xCreateTaskSemaphore;

// 读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

__IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT; 

// 以下是SPI模块的初始化代码，配置成主机模式 						  
// SPI口初始化
// 这里针是对SPI1的初始化
static void SPIx_Init(void)
{	 
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(SPI_SCK_CLK|SPI_MISO_CLK|SPI_MOSI_CLK|SPI_CS_CLK, ENABLE);//使能GPIO时钟
  RCC_APB2PeriphClockCmd(SPI_RCC_CLK, ENABLE);// 使能SPI时钟
 
  // SPI引脚初始化  SCK
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
	
	//	SPI引脚初始化  MISO
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);
	
	//	SPI引脚初始化  MOSI
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
	
	//	SPI引脚初始化  CS
	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(SPI_SCK_PORT,SPI_AF_SCK_PinSourcex,SPI_AF_SPIx); // 复用为 SPI1
	GPIO_PinAFConfig(SPI_MISO_PORT,SPI_AF_MISO_PinSourcex,SPI_AF_SPIx); // 复用为 SPI1
	GPIO_PinAFConfig(SPI_MOSI_PORT,SPI_AF_MOSI_PinSourcex,SPI_AF_SPIx); // 复用为 SPI1

	/* 停止信号 FLASH: CS引脚高电平*/
  SPI_CS_HIGH();
 
	//	SPI初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		// 设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		// 设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		// 串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	// 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		// NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		// 定义波特率预分频的值:波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	// 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	// CRC值计算的多项式
	SPI_Init(SPIx, &SPI_InitStructure);  // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE); // 使能SPI外设	 
}   

/**
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* 等待超时后的处理,输出错误信息 */
  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}

 /**
  * @brief  使用SPI发送一个字节的数据
  * @param  byte：要发送的数据
  * @retval 返回接收到的数据
  */
static u8 SPI_SendByte(u8 byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* 等待发送缓冲区为空，TXE事件 */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  SPI_I2S_SendData(SPIx, byte);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* 等待接收缓冲区非空，RXNE事件 */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* 读取数据寄存器，获取接收缓冲区数据 */
  return SPI_I2S_ReceiveData(SPIx);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
static u16 SPI_SendHalfWord(u16 HalfWord)
{
  
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
   }

  /* Send Half Word through the FLASH_SPI peripheral */
  SPI_I2S_SendData(SPIx, HalfWord);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(3);
   }
  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPIx);
}


void SPI_FLASH_Init(void)
{
	SPIx_Init();
	
	DeviceID = SPI_FLASH_ReadDeviceID();
	delay_ms( 200 );
	/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
}

 /**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  SPI_CS_LOW();
  /* 发送扇区擦除指令*/
  SPI_SendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  SPI_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPI_SendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPI_SendByte(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_CS_HIGH();
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  无
  * @retval 无
  */
void SPI_FLASH_BulkErase(void)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整块 Erase */
  /* 选择FLASH: CS低电平 */
  SPI_CS_LOW();
  /* 发送整块擦除指令*/
  SPI_SendByte(W25X_ChipErase);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_CS_HIGH();

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 选择FLASH: CS低电平 */
  SPI_CS_LOW();
  /* 写页写指令*/
  SPI_SendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  SPI_SendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  SPI_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
    /* 发送当前要写入的字节数据 */
    SPI_SendByte(*pBuffer);
    /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_CS_HIGH();

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*差count个数据值，刚好可以对齐到页地址*/
  count = SPI_FLASH_PageSize - Addr;	
	/*计算出要写多少整数页*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod运算求余，计算出剩余不满一页的字节数*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			
			/*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* 选择FLASH: CS低电平 */
  SPI_CS_LOW();

  /* 发送 读 指令 */
  SPI_SendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPI_SendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPI_SendByte(ReadAddr & 0xFF);
  
	/* 读取数据 */
  while (NumByteToRead--)
  {
    /* 读取一个字节*/
    *pBuffer = SPI_SendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_CS_HIGH();
}


 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 开始通讯：CS低电平 */
  SPI_CS_LOW();

  /* 发送JEDEC指令，读取ID */
  SPI_SendByte(W25X_JedecDeviceID);

  /* 读取一个字节数据 */
  Temp0 = SPI_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp1 = SPI_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp2 = SPI_SendByte(Dummy_Byte);

  /* 停止通讯：CS高电平 */
  SPI_CS_HIGH();

	/*把数据组合起来，作为函数的返回值*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

 /**
  * @brief  读取FLASH Device ID
  * @param 	无
  * @retval FLASH Device ID
  */
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_CS_LOW();

  /* Send "RDID " instruction */
  SPI_SendByte(W25X_DeviceID);
  SPI_SendByte(Dummy_Byte);
  SPI_SendByte(Dummy_Byte);
  SPI_SendByte(Dummy_Byte);
  
  /* Read a byte from the FLASH */
  Temp = SPI_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_CS_HIGH();

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_SendByte(ReadAddr & 0xFF);
}


 /**
  * @brief  使用SPI读取一个字节的数据
  * @param  无
  * @retval 返回接收到的数据
  */
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_SendByte(Dummy_Byte));
}

 /**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* 通讯开始：CS低 */
  SPI_CS_LOW();

  /* 发送写使能命令*/
  SPI_SendByte(W25X_WriteEnable);

  /*通讯结束：CS高 */
  SPI_CS_HIGH();
}

 /**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* 选择 FLASH: CS 低 */
  SPI_CS_LOW();

  /* 发送 读状态寄存器 命令 */
  SPI_SendByte(W25X_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 若FLASH忙碌，则等待 */
  do
  {
    /* 读取FLASH芯片的状态寄存器 */
    FLASH_Status = SPI_SendByte(Dummy_Byte);	 
    {
      if((SPITimeout--) == 0) 
      {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    } 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
  SPI_CS_HIGH();
}


//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  /* 选择 FLASH: CS 低 */
  SPI_CS_LOW();

  /* 发送 掉电 命令 */
  SPI_SendByte(W25X_PowerDown);

  /* 停止信号  FLASH: CS 高 */
  SPI_CS_HIGH();
}   

//唤醒
void SPI_Flash_WAKEUP(void)   
{
  /*选择 FLASH: CS 低 */
  SPI_CS_LOW();

  /* 发上 上电 命令 */
  SPI_SendByte(W25X_ReleasePowerDown);

  /* 停止信号 FLASH: CS 高 */
  SPI_CS_HIGH();                   //等待TRES1
}   

void manual_Collect(const double *collectGPSData)
{
	assert_param(collectGPSData);
	
	static uint8_t sectorEraseFlag = 0;
	static uint16_t writeSectorSize = 100;
	static uint16_t sectorCount = 1;  //擦除扇区序号，因为扇区0被IMU的校准数据占用，所以轨迹点存储在扇区1后面
	static uint16_t addr = 100;//FLASH存储地址
	static uint16_t NUM = 0;
	static POSI_UNION posi;//采用union将实现结构体和字节的相互转换
	static POSI_UNION posi_show;//用于显示

	if(collectGPSData[0] == 0 && collectGPSData[1] == 0)   // 收到两个数据同时为0，认为采集完成
	{
		SPI_FLASH_BufferWrite((uint8_t*)&NUM, FLASH_SECTORx(1)+0, 1);
		printf("wancheng\r\n");
		return;
	}
	
	if(writeSectorSize >= 4096)
	{
		writeSectorSize -= 4096;
		sectorEraseFlag = 0;
		sectorCount++;
		addr = 0;
	}
	else if(4096 - writeSectorSize < sizeof(WGS84_T))
	{
		sectorEraseFlag = 0;
		sectorCount++;
		addr = 0;
	}
	
	if(sectorEraseFlag == 0)
	{
		sectorEraseFlag = 1;
		SPI_FLASH_SectorErase(FLASH_SECTORx(sectorCount));
	}
			
	//当前点距离上一个点达到5则将当前点作为新点，<20的条件是为了防止错误数据被采集
	posi.wgs84.latitude = collectGPSData[1];
	posi.wgs84.longitude = collectGPSData[0];
	SPI_FLASH_BufferWrite(posi.data, FLASH_SECTORx(sectorCount)+addr, sizeof(WGS84_T));
	delay_ms(10);
//	SPI_FLASH_BufferRead((void*)posi_show.data, FLASH_SECTORx(sectorCount)+addr, sizeof(WGS84_T));
//	delay_ms(10);
	
	writeSectorSize += sizeof(WGS84_T);
	addr += sizeof(WGS84_T);
	
	NUM++;
//	printf("NUM ---> %d\r\n",NUM);
//	printf("lon = %.7lf, lat = %.7lf\r\n",posi_show.wgs84.longitude, posi_show.wgs84.latitude);
}


//将点数和所有坐标全部读出，经串口发送至PC，可用matlab程序绘制轨迹
void Flash_Read(void)
{
	uint16_t ADDR = 100;
	static POSI_UNION posi_show;
	static POSI_ST posi_sh;
	uint8_t num0 = 0;
	uint8_t i = 1;
	Get_Num(&num0);
	printf("#%d#\r\n",num0);
	while(i <= num0 && i != 0){
		SPI_FLASH_BufferRead(posi_show.data, FLASH_SECTORx(1)+ADDR, sizeof(WGS84_T));
		delay_ms(10);
		Get_POSI(posi_show.wgs84.longitude, posi_show.wgs84.latitude, &posi_sh);
//		printf("%.2f %.2f\r\n",posi_sh.x,posi_sh.y);
		
		printf("%.7f  %.7f\r\n",posi_show.wgs84.longitude,posi_show.wgs84.latitude);
		i++;
		ADDR += sizeof(WGS84_T);
	}
}

//获取Flash中的第i个坐标
void Flash_Get_POSI(POSI_ST* posi,uint16_t i)
{
	SPI_FLASH_BufferRead(posi_now.data, FLASH_SECTORx(1)+100 + i*sizeof(WGS84_T), sizeof(WGS84_T));//100为开始存点的地址
//	printf("1111longitude = %lf,latitude = %lf\r\n",posi_now.wgs84.longitude,posi_now.wgs84.latitude);
	delay_ms(10);
	Get_POSI(posi_now.wgs84.longitude, posi_now.wgs84.latitude, posi);//经纬度转为笛卡尔坐标
}

//获取采集的点数
void Get_Num(uint8_t* n)
{
	SPI_FLASH_BufferRead(n, FLASH_SECTORx(1)+0, 1);	//点数存在0x00地址处
}

