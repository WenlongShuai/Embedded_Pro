#include "W25Qxx.h"
#include "key.h"
#include "FreeRTOS.h"
#include "semphr.h"  // �ź���������ͺͺ���������

POSI_UNION posi_now = {0};

// 
extern SemaphoreHandle_t xCreateTaskSemaphore;

// ��ȡ��ID�洢λ��
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

__IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT; 

// ������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
// SPI�ڳ�ʼ��
// �������Ƕ�SPI1�ĳ�ʼ��
static void SPIx_Init(void)
{	 
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(SPI_SCK_CLK|SPI_MISO_CLK|SPI_MOSI_CLK|SPI_CS_CLK, ENABLE);//ʹ��GPIOʱ��
  RCC_APB2PeriphClockCmd(SPI_RCC_CLK, ENABLE);// ʹ��SPIʱ��
 
  // SPI���ų�ʼ��  SCK
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
	
	//	SPI���ų�ʼ��  MISO
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);
	
	//	SPI���ų�ʼ��  MOSI
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
	
	//	SPI���ų�ʼ��  CS
	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(SPI_SCK_PORT,SPI_AF_SCK_PinSourcex,SPI_AF_SPIx); // ����Ϊ SPI1
	GPIO_PinAFConfig(SPI_MISO_PORT,SPI_AF_MISO_PinSourcex,SPI_AF_SPIx); // ����Ϊ SPI1
	GPIO_PinAFConfig(SPI_MOSI_PORT,SPI_AF_MOSI_PinSourcex,SPI_AF_SPIx); // ����Ϊ SPI1

	/* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
  SPI_CS_HIGH();
 
	//	SPI��ʼ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		// ����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		// ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		// ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	// ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		// NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		// ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	// ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	// CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &SPI_InitStructure);  // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPIx, ENABLE); // ʹ��SPI����	 
}   

/**
  * @brief  �ȴ���ʱ�ص�����
  * @param  None.
  * @retval None.
  */
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  FLASH_ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}

 /**
  * @brief  ʹ��SPI����һ���ֽڵ�����
  * @param  byte��Ҫ���͵�����
  * @retval ���ؽ��յ�������
  */
static u8 SPI_SendByte(u8 byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
  SPI_I2S_SendData(SPIx, byte);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ջ������ǿգ�RXNE�¼� */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* ��ȡ���ݼĴ�������ȡ���ջ��������� */
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
	/* ��ȡ SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
}

 /**
  * @brief  ����FLASH����
  * @param  SectorAddr��Ҫ������������ַ
  * @retval ��
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* �������� */
  /* ѡ��FLASH: CS�͵�ƽ */
  SPI_CS_LOW();
  /* ������������ָ��*/
  SPI_SendByte(W25X_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  SPI_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  SPI_SendByte((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  SPI_SendByte(SectorAddr & 0xFF);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  SPI_CS_HIGH();
  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ����FLASH��������Ƭ����
  * @param  ��
  * @retval ��
  */
void SPI_FLASH_BulkErase(void)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ���� Erase */
  /* ѡ��FLASH: CS�͵�ƽ */
  SPI_CS_LOW();
  /* �����������ָ��*/
  SPI_SendByte(W25X_ChipErase);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  SPI_CS_HIGH();

  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * @retval ��
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ѡ��FLASH: CS�͵�ƽ */
  SPI_CS_LOW();
  /* дҳдָ��*/
  SPI_SendByte(W25X_PageProgram);
  /*����д��ַ�ĸ�λ*/
  SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  SPI_SendByte((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  SPI_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
    /* ���͵�ǰҪд����ֽ����� */
    SPI_SendByte(*pBuffer);
    /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  SPI_CS_HIGH();

  /* �ȴ�д�����*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param  WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
  count = SPI_FLASH_PageSize - Addr;	
	/*�����Ҫд��������ҳ*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*�Ȱ�����ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* ����ַ�� SPI_FLASH_PageSize ������  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��д����*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*��д����ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*��дʣ�������*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*��ַ����������count�ֿ������������������*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*������ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* ѡ��FLASH: CS�͵�ƽ */
  SPI_CS_LOW();

  /* ���� �� ָ�� */
  SPI_SendByte(W25X_ReadData);

  /* ���� �� ��ַ��λ */
  SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  SPI_SendByte((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  SPI_SendByte(ReadAddr & 0xFF);
  
	/* ��ȡ���� */
  while (NumByteToRead--)
  {
    /* ��ȡһ���ֽ�*/
    *pBuffer = SPI_SendByte(Dummy_Byte);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  SPI_CS_HIGH();
}


 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* ��ʼͨѶ��CS�͵�ƽ */
  SPI_CS_LOW();

  /* ����JEDECָ���ȡID */
  SPI_SendByte(W25X_JedecDeviceID);

  /* ��ȡһ���ֽ����� */
  Temp0 = SPI_SendByte(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp1 = SPI_SendByte(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp2 = SPI_SendByte(Dummy_Byte);

  /* ֹͣͨѶ��CS�ߵ�ƽ */
  SPI_CS_HIGH();

	/*�����������������Ϊ�����ķ���ֵ*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

 /**
  * @brief  ��ȡFLASH Device ID
  * @param 	��
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
  * @brief  ʹ��SPI��ȡһ���ֽڵ�����
  * @param  ��
  * @retval ���ؽ��յ�������
  */
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_SendByte(Dummy_Byte));
}

 /**
  * @brief  ��FLASH���� дʹ�� ����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* ͨѶ��ʼ��CS�� */
  SPI_CS_LOW();

  /* ����дʹ������*/
  SPI_SendByte(W25X_WriteEnable);

  /*ͨѶ������CS�� */
  SPI_CS_HIGH();
}

 /**
  * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* ѡ�� FLASH: CS �� */
  SPI_CS_LOW();

  /* ���� ��״̬�Ĵ��� ���� */
  SPI_SendByte(W25X_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* ��FLASHæµ����ȴ� */
  do
  {
    /* ��ȡFLASHоƬ��״̬�Ĵ��� */
    FLASH_Status = SPI_SendByte(Dummy_Byte);	 
    {
      if((SPITimeout--) == 0) 
      {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    } 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* ����д���־ */

  /* ֹͣ�ź�  FLASH: CS �� */
  SPI_CS_HIGH();
}


//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  /* ѡ�� FLASH: CS �� */
  SPI_CS_LOW();

  /* ���� ���� ���� */
  SPI_SendByte(W25X_PowerDown);

  /* ֹͣ�ź�  FLASH: CS �� */
  SPI_CS_HIGH();
}   

//����
void SPI_Flash_WAKEUP(void)   
{
  /*ѡ�� FLASH: CS �� */
  SPI_CS_LOW();

  /* ���� �ϵ� ���� */
  SPI_SendByte(W25X_ReleasePowerDown);

  /* ֹͣ�ź� FLASH: CS �� */
  SPI_CS_HIGH();                   //�ȴ�TRES1
}   

void manual_Collect(const double *collectGPSData)
{
	assert_param(collectGPSData);
	
	static uint8_t sectorEraseFlag = 0;
	static uint16_t writeSectorSize = 100;
	static uint16_t sectorCount = 1;  //����������ţ���Ϊ����0��IMU��У׼����ռ�ã����Թ켣��洢������1����
	static uint16_t addr = 100;//FLASH�洢��ַ
	static uint16_t NUM = 0;
	static POSI_UNION posi;//����union��ʵ�ֽṹ����ֽڵ��໥ת��
	static POSI_UNION posi_show;//������ʾ

	if(collectGPSData[0] == 0 && collectGPSData[1] == 0)   // �յ���������ͬʱΪ0����Ϊ�ɼ����
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
			
	//��ǰ�������һ����ﵽ5�򽫵�ǰ����Ϊ�µ㣬<20��������Ϊ�˷�ֹ�������ݱ��ɼ�
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


//����������������ȫ�������������ڷ�����PC������matlab������ƹ켣
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

//��ȡFlash�еĵ�i������
void Flash_Get_POSI(POSI_ST* posi,uint16_t i)
{
	SPI_FLASH_BufferRead(posi_now.data, FLASH_SECTORx(1)+100 + i*sizeof(WGS84_T), sizeof(WGS84_T));//100Ϊ��ʼ���ĵ�ַ
//	printf("1111longitude = %lf,latitude = %lf\r\n",posi_now.wgs84.longitude,posi_now.wgs84.latitude);
	delay_ms(10);
	Get_POSI(posi_now.wgs84.longitude, posi_now.wgs84.latitude, posi);//��γ��תΪ�ѿ�������
}

//��ȡ�ɼ��ĵ���
void Get_Num(uint8_t* n)
{
	SPI_FLASH_BufferRead(n, FLASH_SECTORx(1)+0, 1);	//��������0x00��ַ��
}

