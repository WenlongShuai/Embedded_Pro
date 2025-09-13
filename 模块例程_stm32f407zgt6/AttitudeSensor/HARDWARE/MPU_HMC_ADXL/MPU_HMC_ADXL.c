#include "MPU_HMC_ADXL.h"

extern __IO uint32_t FlashID;

uint8_t RecvBuff[RECVBUFF_SIZE];

/* HMC5883L */
/*------------------------- 校准参数 --------------------------*/
short x_offset = 0;       // X轴零点偏移补偿
short y_offset = 0;       // Y轴零点偏移补偿
float x_scale = 1.0;      // X轴比例系数（椭圆校准用）
float y_scale = 1.0;      // Y轴比例系数（椭圆校准用）
unsigned char HMC_BUF[8] = {0};     // 数据缓冲区（存储6轴数据+2预留）

/* MPU3050 */
unsigned char TX_DATA[4];           //显示据缓存区
unsigned char MPU_BUF[10] = {0};       //接收数据缓存区


/* ADXL345 */
unsigned char ADXL_BUF[6] = {0};


#ifndef SOFTWARE_IIC  //硬件I2C
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;
#endif


void MPU_HMC_ADXL_Init(void)
{
	#ifdef SOFTWARE_IIC  //软件I2C
	IIC_Init();
	#else  //硬件I2C
	I2C_Mode_Config();
	#endif
	
//	I2C1_DMA_Init();
	
	Init_HMC5883();
	Init_MPU3050();
	Init_ADXL345();
	SPI_FLASH_Init();
}

/*------------------- IIC 单字节写操作 -------------------*/
void IIC_Single_Write(unsigned char Slave_Address, unsigned char REG_Address, unsigned short Len,unsigned char *Data_Buf)
{
	// 写寄存器操作
	
	#ifdef SOFTWARE_IIC  //软件I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	
	for(int i = 0;i < Len;i++)
	{
		IIC_Send_Byte(REG_Address+i);
		IIC_Wait_Ack();
		IIC_Send_Byte(Data_Buf[i]);
		IIC_Wait_Ack();
	}
	IIC_Stop();
	#else   //硬件I2C
	/* Send STRAT condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
	I2CTimeout = I2CT_FLAG_TIMEOUT; 
	
	/* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return;
  } 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);
	
	/* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }  
	
	
	for(int i = 0;i < Len;i++)
	{
		/* Send internal address to write to */
		I2C_SendData(IIC_I2Cx, REG_Address+i);
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((I2CTimeout--) == 0) return;
		} 
		
		/* Send the byte to be written */
		I2C_SendData(IIC_I2Cx, Data_Buf[i]); 
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;  
		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((I2CTimeout--) == 0) return;
		} 
	}
  
  /* Send STOP condition */
  I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	#endif
}

unsigned char IIC_Single_Read(unsigned char Slave_Address, unsigned char REG_Address)
{
	unsigned char REG_data = 0;
	
	// 读寄存器操作
	#ifdef SOFTWARE_IIC  //软件I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	REG_data = IIC_Read_Byte(0);
	IIC_Stop();
	#else   //硬件I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;
  
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
  {
    if((I2CTimeout--) == 0) return 0;
  }
  
  /* Send START condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return 0;
  }
  
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(IIC_I2Cx, ENABLE);

  /* Send internal address to write to */
  I2C_SendData(IIC_I2Cx, REG_Address);  
   
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return 0;
  }
    
  /* Send address for read */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return 0;
   }
  
	 /* Disable Acknowledgement */
	I2C_AcknowledgeConfig(IIC_I2Cx, DISABLE);
	 
	/* Test on EV7 and clear it */    
	I2CTimeout = I2CT_LONG_TIMEOUT;
	while(I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
	{
		if((I2CTimeout--) == 0) return 0;
	}     
	
	/* Read a byte */
	REG_data = I2C_ReceiveData(IIC_I2Cx);
	
	/* Send STOP Condition */
	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	#endif
	
  return REG_data;
}

void IIC_Multiple_Read(unsigned char Slave_Address, unsigned char REG_Address, unsigned short size ,unsigned char *readBuf)
{
	// 读寄存器操作
	#ifdef SOFTWARE_IIC  //软件I2C
	IIC_Start();
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	
	for(int i = 0;i < 6;i++)
	{
		if(i == 5)	
			readBuf[i] = IIC_Read_Byte(0);
		else
			readBuf[i] = IIC_Read_Byte(1);	
	}
	IIC_Stop();
	#else  //硬件I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;

	//*((u8 *)0x4001080c) |=0x80; 
	while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send START condition */
	I2C_GenerateSTART(IIC_I2Cx, ENABLE);
	//*((u8 *)0x4001080c) &=~0x80;

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send address for write */
	I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Clear EV6 by setting again the PE bit */
	I2C_Cmd(IIC_I2Cx, ENABLE);

	/* Send internal address to write to */
	I2C_SendData(IIC_I2Cx, REG_Address);  

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send STRAT condition a second time */  
	I2C_GenerateSTART(IIC_I2Cx, ENABLE);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* Send address for read */
	I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return;
	}

	/* While there is data to be read */
	for(int i = 0;i < size;i++)
	{
		if(i == 5)
		{
			/* Disable Acknowledgement */
			I2C_AcknowledgeConfig(IIC_I2Cx, DISABLE);
		}

		/* Test on EV7 and clear it */    
		I2CTimeout = I2CT_LONG_TIMEOUT;

		while(I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return;
		}      
		/* Read a byte */
		readBuf[i] = I2C_ReceiveData(IIC_I2Cx);  
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	/* Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	#endif
}

void IIC_Read_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size)
{
	#if 0
	// 读寄存器操作
	#ifdef SOFTWARE_IIC  //软件I2C
	IIC_Start();
	/* 配置DMA接收缓冲区大小 */
  DMA1_Stream0->NDTR = RECVBUFF_SIZE;
	IIC_Send_Byte(Slave_Address);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(Slave_Address+1);
	IIC_Wait_Ack();
	
	/* 清除接收完成标志 */
	I2C1_RxComplete = 0;
	
	/* 使能DMA接收 */
	DMA_Cmd(DMA1_Stream0, ENABLE);
		
	IIC_Stop();
	#else  //硬件I2C
	I2CTimeout = I2CT_LONG_TIMEOUT;
  
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(IIC_I2Cx, I2C_FLAG_BUSY))
  {
    if((I2CTimeout--) == 0) return;
  }

  /* Send START condition */
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return;
  }
  
  /* Send address for write */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(IIC_I2Cx, ENABLE);

  /* Send internal address to write to */
  I2C_SendData(IIC_I2Cx, REG_Address);  
   
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(IIC_I2Cx, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return;
  }
    
  /* Send address for read */
  I2C_Send7bitAddress(IIC_I2Cx, Slave_Address+1, I2C_Direction_Receiver);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }
	
	/* 清除接收完成标志 */
	I2C1_RxComplete = 0;
	
	/* 使能DMA接收 */
	DMA_Cmd(DMA1_Stream0, ENABLE);

	/* Send STOP Condition */
//	I2C_GenerateSTOP(IIC_I2Cx, ENABLE);
	
	/* Enable Acknowledgement to be ready for another reception */
	//I2C_AcknowledgeConfig(IIC_I2Cx, ENABLE);
	
	#endif
	
	#endif
	
// /* 检查设备连接状态 */
//	uint8_t status;
//	status = I2C1_CheckConnection(Slave_Address);
//	if(status != 0)
//	{
//			/* 处理连接错误 */
//			//while(1)
//				printf("I2C Read Connection ERROR\r\n");
//				return;
//	}

	/* 连续读取10个寄存器的数据 */

	I2C1_DMA_ReceiveData(Slave_Address, REG_Address, data, size);
}

void IIC_Write_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size)
{
 /* 检查设备连接状态 */
	uint8_t status;
 /* 检查设备连接状态 */
	status = I2C1_CheckConnection(Slave_Address);
	if(status != 0)
	{
			/* 处理连接错误 */
			//while(1)
			{
				printf("I2C Write Connection ERROR %d\r\n",status);
				return;
				
			}
	}
	
	/* 发送数据 */
	status = I2C1_DMA_SendData(Slave_Address, &REG_Address, 1);
	if(status != 0)
	{
			/* 处理发送错误 */
			//while(1)
			{
				printf("I2C DMA Write EEROR  %d\r\n",status);
				return;
			}
	}
	
	/* 发送数据 */
	status = I2C1_DMA_SendData(Slave_Address, data, 1);
	if(status != 0)
	{
			/* 处理发送错误 */
			//while(1)
			{
				printf("I2C DMA Write EEROR  %d\r\n",status);
				return;
			}
	}
}

/*------------------- HMC5883 传感器初始化配置 ----------------------*/
/*
寄存器配置解析：
- 配置寄存器A（0x00）: 
  (3<<5)=0b1100000 → 8次采样平均（数据手册建议值）
  (1<<4)=0b010000 → 15Hz输出速率
  0 → 正常测量模式（非自检）
- 配置寄存器B（0x01）: 
  (1<<5)=0b100000 → ±1.3Ga量程（默认灵敏度1090 LSB/Gauss）
- 模式寄存器（0x02）: 
  0x00 → 连续测量模式
*/
void Init_HMC5883()
{
	unsigned char HMC_InitBuf[] = {0x70, 0xE0, 0x00};
	Delay_us(100);
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_InitBuf[0]); // 配置寄存器A
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_B, 1, &HMC_InitBuf[1]);       // 配置寄存器B
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_InitBuf[2]);           // 模式寄存器
	
	
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_CONFIG_A, &HMC_InitBuf[0], 2); // 配置寄存器A
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_CONFIG_B, &HMC_InitBuf[1], 2);       // 配置寄存器B
//	IIC_Write_DMA(HMC_SlaveAddress, HMC5883_MODE, &HMC_InitBuf[2], 2);           // 模式寄存器
	
	// 初始化卡尔曼滤波器
	float q = 0.1;  // 过程噪声协方差
	float r = 0;    // 测量噪声协方差
	float initial_x = 0;  // 初始状态估计值
	float initial_p = 0;  // 初始估计误差协方差
}

/*------------------- 自检模式实现 -------------------------*/
/*
自检流程：
1. 配置为内部正激励模式（产生已知磁场）
2. 切换为单次测量模式
3. 读取测试值（正常范围X/Y≈951，Z≈886）
4. 恢复正常工作模式
*/
unsigned char Init_HMC5883_SelfTestMode() 
{
	unsigned char HMC_SelfTestBuf[] = {0x70|0x01, 0x01, 0x70|0x00, 0x00};
	
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_SelfTestBuf[0]); // 配置寄存器A（0b01110001）
	Delay_ms(10);
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_SelfTestBuf[1]);     // 单次测量模式                     
	Delay_ms(2000); // 等待数据稳定
	IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF);              // 读取自检数据

	// 恢复正常模式
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_CONFIG_A, 1, &HMC_SelfTestBuf[2]); 
	IIC_Single_Write(HMC_SlaveAddress, HMC5883_MODE, 1, &HMC_SelfTestBuf[3]);
	return 0;
}

/*------------------- 椭圆校准算法 ------------------------*/
/*
校准步骤：
1. 360°旋转设备采集X/Y极值
2. 计算零点偏移：offset = (max + min)/2
3. 计算比例因子：scale = avg_radius / axis_radius
*/
void CalibrateHMC5883() 
{
	short x_min = 0x7FFF, x_max = 0x8000;
	short y_min = 0x7FFF, y_max = 0x8000;
	uint8_t cal_flag = 0;
	float HMC_CalFactor[5] = {0};  //第[0]个表示是否写入校准系数
	
	if(FlashID == sFLASH_ID)
	{
		/*读取数据*/
		SPI_FLASH_BufferRead((void*)HMC_CalFactor, SPI_FLASH_PageSize*0, sizeof(HMC_CalFactor));
		
		if(HMC_CalFactor[0] == 0xBB)  /*若标志等于0xcd，表示之前已有写入数据*/
		{
			x_offset = HMC_CalFactor[1];
			y_offset = HMC_CalFactor[3];
			x_scale = HMC_CalFactor[2];
			y_scale = HMC_CalFactor[4];
			SPI_Flash_PowerDown(); 
			printf("已被校准！\n");
			return;
		}

		printf("请缓慢360°旋转\n");
		for (int i = 0; i < 120; i++)
		{
			Delay_ms(100);
			IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF);
//			IIC_Read_DMA(HMC_SlaveAddress, HMC5883_DATA_X_H, HMC_BUF, 6); 	// 获取原始数据
			
			short x = (short)(HMC_BUF[0]  << 8) | HMC_BUF[1];
			short y = (short)(HMC_BUF[4]  << 8) | HMC_BUF[5];
			
			printf("x --> %d\r\n",x);
			printf("y --> %d\r\n",y);
			
			// 更新极值
			x_min = (x < x_min) ? x : x_min;
			x_max = (x > x_max) ? x : x_max;
			y_min = (y < y_min) ? y : y_min;
			y_max = (y > y_max) ? y : y_max;
		}

		// 计算校准参数
		x_offset = (x_max + x_min) / 2;
		y_offset = (y_max + y_min) / 2;
		float avg_radius = ((x_max - x_min) + (y_max - y_min)) / 4.0;
		x_scale = avg_radius / (x_max - x_min);
		y_scale = avg_radius / (y_max - y_min);

		HMC_CalFactor[0] = 0xBB;  //0xCD是否写入校准系数
		HMC_CalFactor[1] = x_offset*1.0;
		HMC_CalFactor[2] = x_scale;
		HMC_CalFactor[3] = y_offset*1.0;
		HMC_CalFactor[4] = y_scale;
		
		printf("校准完成！\n");
		
		/* 将校准系数写入Flash */
		//擦除扇区
		SPI_FLASH_SectorErase(0);
		SPI_FLASH_BufferWrite((void*)HMC_CalFactor, SPI_FLASH_PageSize*0, sizeof(HMC_CalFactor));
		SPI_Flash_PowerDown(); 
	}
}

/*------------------- 主数据采集流程 ----------------------*/
void ReadData_HMC5883(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(HMC_SlaveAddress, HMC5883_DATA_X_H, 6, HMC_BUF); // 获取原始数据
	// 原始数据解析（注意X/Y寄存器顺序）
	*x_raw = (HMC_BUF[0]  << 8) | HMC_BUF[1];		//读取计算X轴数据
	//printf("%f\t",*x_raw);
	*y_raw = (HMC_BUF[4]  << 8) | HMC_BUF[5];		//读取计算X轴数据
	//printf("%f\t",*y_raw);
	*z_raw = (HMC_BUF[2]  << 8) | HMC_BUF[3];		//读取计算X轴数据
	//printf("%f\r\n",*z_raw);

	//数据输出
	//printf("x = %d y = %d\r\n", x, y);
}

void ReadData_HMC5883_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(HMC_SlaveAddress, HMC5883_DATA_X_H, RecvBuff, 6); // 获取原始数据
	// 原始数据解析（注意X/Y寄存器顺序）
	*x_raw = (RecvBuff[0]  << 8) | RecvBuff[1];		//读取计算X轴数据
	//printf("%f\t",*x_raw);
	*y_raw = (RecvBuff[4]  << 8) | RecvBuff[5];		//读取计算X轴数据
	//printf("%f\t",*y_raw);
	*z_raw = (RecvBuff[2]  << 8) | RecvBuff[3];		//读取计算X轴数据
	//printf("%f\r\n",*z_raw);

	//数据输出
	//printf("x = %d y = %d\r\n", x, y);
}

/*------------------- 方位角计算 --------------------------*/
// 将-180°~180°转换为0°~360°，并映射到8个主方向
double AngleToDirection(short x_raw, short y_raw, short z_raw) 
{
	// 应用校准补偿
	short x = (x_raw - x_offset) * x_scale;
	short y = (y_raw - y_offset) * y_scale;

	// 方位角计算（注意atan2参数顺序为y,x）
	double angle = atan2((double)y, (double)x) * (180.0 / 3.14159265358979);
	if (angle < 0) angle += 360;         // 转换为0-360°
	
//	const float sector = 45.0;            // 每个方向45°范围
//	const char* directions[] = {"北", "东北", "东", "东南","南", "西南", "西", "西北"};
//	int index = (int)((angle + sector/2) / sector) % 8;								 
//	return directions[index];
	
	return angle;
}


//初始化MPU3050，根据需要请参考pdf进行修改
static void Init_MPU3050(void)
{
	unsigned char MPU_InitBuf[] = {0x80, 0x03, 0x07, 0x1E};
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_PWR_M, 1, &MPU_InitBuf[0]);	     //解除休眠状态  软复位
	Delay_us(100);   //等待复位稳定
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_PWR_M, 1, &MPU_InitBuf[1]);	     // 配置时钟源为PLL_Z
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_SMPL, 1, &MPU_InitBuf[2]);    //陀螺仪采样率
	IIC_Single_Write(MPU_SlaveAddress, MPU3050_RA_DLPF, 1, &MPU_InitBuf[3]);    // 配置低通滤波器和量程（42Hz带宽，±2000°/s）
	
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_PWR_M, &MPU_InitBuf[0], 2);	     //解除休眠状态  软复位
//	Delay_us(100);   //等待复位稳定
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_PWR_M, &MPU_InitBuf[1], 2);	     // 配置时钟源为PLL_Z
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_SMPL, &MPU_InitBuf[2], 2);    //陀螺仪采样率
//	IIC_Write_DMA(MPU_SlaveAddress, MPU3050_RA_DLPF, &MPU_InitBuf[3], 2);    // 配置低通滤波器和量程（42Hz带宽，±2000°/s）
}
        
//******读取MPU3050数据  I2C方式****************************************
void ReadData_MPU3050(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(MPU_SlaveAddress, MPU3050_RA_GX_H, 6, MPU_BUF);
	*x_raw = (MPU_BUF[0]<<8)|MPU_BUF[1];		//读取计算X轴数据
	//printf("%f\t",*x_raw);
	*y_raw = (MPU_BUF[2]<<8)|MPU_BUF[3]; //读取计算Y轴数据
	//printf("%f\t",*y_raw);
	*z_raw = (MPU_BUF[4]<<8)|MPU_BUF[5];   //读取计算Z轴数据
	//printf("%f\t\r\n",*z_raw);	

	//数据输出
	//printf("陀螺仪x = %d 陀螺仪y = %d 陀螺仪z = %d\r\n", T_X, T_Y, T_Z);
}

//******读取MPU3050数据  DMA方式****************************************
void ReadData_MPU3050_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(MPU_SlaveAddress, MPU3050_RA_GX_H, RecvBuff, 6);
	short temp = 0;
	*x_raw = (RecvBuff[0]<<8)|RecvBuff[1];		//读取计算X轴数据
	//printf("%f\t",*x_raw);
	*y_raw = (RecvBuff[2]<<8)|RecvBuff[3]; //读取计算Y轴数据
	//printf("%f\t",*y_raw);
	*z_raw = (RecvBuff[4]<<8)|RecvBuff[5];   //读取计算Z轴数据
	//printf("%f\t\r\n",*z_raw);	
	//数据输出
	//printf("陀螺仪x = %d 陀螺仪y = %d 陀螺仪z = %d\r\n", T_X, T_Y, T_Z);
}


/* ADXL345 */
static void Init_ADXL345(void) 
{
	unsigned char ADXL_InitBuf[] = {0X0B, 0x0A, 0x08, 0x00, 0x00, 0x00, 0xF0};
	Delay_ms(2);  //上电等待
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_POWER_CTL,1,&ADXL_InitBuf[2]);		//测量模式
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_DATA_FORMAT,1,&ADXL_InitBuf[0]);		//13位全分辨率,16g量程 
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_BW_RATE,1,&ADXL_InitBuf[1]);		//数据输出速度为100Hz
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_INT_ENABLE,1,&ADXL_InitBuf[3]);		//不使用中断		 
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSX,1,&ADXL_InitBuf[4]);
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSY,1,&ADXL_InitBuf[5]);
	IIC_Single_Write(ADXL_SlaveAddress,ADXL345_OFSZ,1,&ADXL_InitBuf[6]);
	
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_POWER_CTL,&ADXL_InitBuf[2],2);		//测量模式
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_DATA_FORMAT,&ADXL_InitBuf[0],2);		//13位全分辨率,16g量程 
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_BW_RATE,&ADXL_InitBuf[1],2);		//数据输出速度为100Hz
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_INT_ENABLE,&ADXL_InitBuf[3],2);		//不使用中断		 
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSX,&ADXL_InitBuf[4],2);
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSY,&ADXL_InitBuf[5],2);
//	IIC_Write_DMA(ADXL_SlaveAddress,ADXL345_OFSZ,&ADXL_InitBuf[6],2);
}


void ReadData_ADXL345(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Multiple_Read(ADXL_SlaveAddress, ADXL345_DATAX0, 6, ADXL_BUF);
	
	*x_raw = ADXL_BUF[1] << 8 | ADXL_BUF[0];
	//printf("%f\t",*x_raw);
	
	*y_raw = ADXL_BUF[3] << 8 | ADXL_BUF[2];
	//printf("%f\t",*y_raw);
	
	*z_raw = ADXL_BUF[5] << 8 | ADXL_BUF[4];
	//printf("%f\r\n",*z_raw);

	// 数据输出
	//printf("加速度计x = %d 加速度计y = %d 加速度计z = %d\r\n", x, y,z);
}

void ReadData_ADXL345_DMA(float *x_raw, float *y_raw, float *z_raw)
{
	IIC_Read_DMA(ADXL_SlaveAddress, ADXL345_DATAX0, RecvBuff, 6);
	
	*x_raw = RecvBuff[1] << 8 | RecvBuff[0];
	//printf("%f\t",*x_raw);
	*y_raw = RecvBuff[3] << 8 | RecvBuff[2];
	//printf("%f\t",*y_raw);
	*z_raw = RecvBuff[5] << 8 | RecvBuff[4];
	//printf("%f\r\n",*z_raw);

	// 数据输出
	//printf("加速度计x = %d 加速度计y = %d 加速度计z = %d\r\n", x, y,z);
}

