#include "bsp_sd_test.h"


uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;



/**
  * @brief  SD卡擦除测试，擦除指定地址的内存。通过读取这些地址的数据来进行比较，看是否擦除成功
  * @param  无
  * @retval 无
  */
void SD_EraseTest(void)
{
  if (Status == SD_OK)
  {
    /* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));  //从0x00地址擦除 NUMBER_OF_BLOCKS 个 BLOCK_SIZE 块字节大小的空间
  }

  if (Status == SD_OK)
  {
		//读取多个块数据
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    
		//等待DMA数据传输
    Status = SD_WaitReadOperation();
	
    //等待SD卡数据接收完成
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
	
  if (Status == SD_OK)
  {
		//检查擦除块的正确性
    EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(EraseStatus == PASSED)
  {
		//擦除成功点亮绿灯
    LED_G(ON);
		printf("SD卡擦除测试成功！\n");
  }
  else
  {
		//擦除失败点亮红灯
    LED_R(ON);  
		printf("SD卡擦除测试失败！\n");
    printf("温馨提示：部分SD卡不支持擦除测试，若SD卡能通过下面的single读写测试，即表示SD卡能够正常使用。\n");
  }
}

/**
  * @brief  SD卡单块读写测试
  * @param  无
  * @retval 无
  */
void SD_SingleBlockTest(void)
{
  /*------------------- Block Read/Write --------------------------*/
  //填充需要发送的buffer空间
  Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);

  if (Status == SD_OK)
  {
    //从0x00地址写入一个块（512byte）大小的数据
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
		
    //等待DMA数据传输
    Status = SD_WaitWriteOperation();
		
		//等待SD卡接收数据完成
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
		//从0x00地址读取一个块大小的数据
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
    
		//等待DMA数据传输
    Status = SD_WaitReadOperation();
		
		//判断SD卡数据是否传输成功
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
		//检查写入块数据的正确性
    TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE);
  }
  
  if(TransferStatus1 == PASSED)
  {
    LED_G(ON);
		printf("Single block 测试成功！\n");
  }
  else
  {
    LED_R(ON);   
		printf("Single block 测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试！\n");
  }
}

/**
  * @brief  SD卡多个块读写测试
  * @param  无
  * @retval 无
  */
void SD_MultiBlockTest(void)
{
  /*--------------- Multiple Block Read/Write ---------------------*/
  //填充即将写入块的数据buffer
  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

  if (Status == SD_OK)
  {
    //从0x00地址写入多个块（512byte）大小的数据
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    
		//等待DMA数据传输
    Status = SD_WaitWriteOperation();
		
		//等待SD接收数据
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
    //从0x00地址读取多个块（512byte）大小的数据
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    
		//等待DMA数据传输
    Status = SD_WaitReadOperation();
		
		//判断SD卡是否接收到数据
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
		//检查多个块写入数据的正确性
    TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(TransferStatus2 == PASSED)
  {
    LED_G(ON);
		printf("Multi block 测试成功！");
  }
  else
  {
    LED_R(ON);    
		printf("Multi block 测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试！");
  }
}

/**
  * @brief  比较两个buffer的数据
  * @param  pBuffer1：第一个buffer数据的地址 
						pBuffer2: 第二个buffer数据的地址
  * @param  BufferLength: buffer的长度
  * @retval PASSED: pBuffer1 和 pBuffer2 相等
  *         FAILED: pBuffer1 和 pBuffer2 不相等
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

/**
  * @brief  填充数据缓冲区
  * @param  pBuffer: 指向要填充的缓冲区
  * @param  BufferLength: 填充缓冲区的大小
  * @param  Offset:在缓冲区上填充的第一个值
  * @retval 无
  */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = index + Offset;
  }
}

/**
  * @brief  检查一个缓冲区是否所有的值都等于0
  * @param  pBuffer: 需要比较的缓冲区
  * @param  BufferLength: buffer的大小
  * @retval PASSED: 缓冲区的值全部为0
  *         FAILED: 缓冲区的值至少有一个不等于0
  */
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    //在一些SD卡中，擦除状态是0xFF，在其他SD卡中是0x00
    if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))
    {
      return FAILED;
    }

    pBuffer++;
  }

  return PASSED;
}

/**
  * @brief  SD测试的主程序
  * @param  无
  * @retval 无
  */
void SDCardTest(void)
{
  /*------------------------------ SD Init ---------------------------------- */
	//SD卡的卡识别模式完成，也将SDIO_CK的频率设置为24MHz，数据线宽度为4bit
  if((Status = SD_EnterCardIdentificationMode()) != SD_OK)
  {
    LED_R(ON);  //如果初始化失败则亮红灯
		printf("SDCard 初始化失败,Status = %d\r\n",Status);
		return ;
  }
	else
	{
		LED_G(ON);
		printf("SDCard 初始化成功\r\n");
	}
      
	if(Status == SD_OK)
	{
		/* 针对不同厂商的 SDCard 有些可能会擦除失败，会一直卡在擦除里面  */
		//LED_B(ON);
		//printf("SDCard 开始擦除测试\r\n");
		//SD_EraseTest();
		
		printf("SDCard 开始单块读写测试\r\n");
		SD_SingleBlockTest();
		
		printf("SDCard 开始多块读写测试\r\n");
		SD_MultiBlockTest();
	}
}



