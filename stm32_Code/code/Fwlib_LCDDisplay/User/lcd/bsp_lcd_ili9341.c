#include "bsp_lcd_ili9341.h"


static void LCD_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LCD_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* 液晶触摸接口 */
	GPIO_InitStruct.GPIO_Pin = LCD_TP_DCLK_PIN;
	GPIO_Init(LCD_TP_DCLK_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_TP_DIN_PIN;
	GPIO_Init(LCD_TP_DIN_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_TP_CS_PIN;
	GPIO_Init(LCD_TP_CS_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_TP_PENIRQ_PIN;
	GPIO_Init(LCD_TP_PENIRQ_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = LCD_TP_DOUT_PIN;
	GPIO_Init(LCD_TP_DOUT_PORT, &GPIO_InitStruct);
	
	
	/* 液晶控制接口 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = LCD_RST_PIN;
	GPIO_Init(LCD_RST_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_BL_PIN;
	GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = LCD_CS_PIN;
	GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_RD_PIN;
	GPIO_Init(LCD_RD_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_WE_PIN;
	GPIO_Init(LCD_WE_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_RS_PIN;
	GPIO_Init(LCD_RS_PORT, &GPIO_InitStruct);
	
	/* 液晶数据接口 */
	GPIO_InitStruct.GPIO_Pin = LCD_DB00_PIN;
	GPIO_Init(LCD_DB00_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB01_PIN;
	GPIO_Init(LCD_DB01_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB02_PIN;
	GPIO_Init(LCD_DB02_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB03_PIN;
	GPIO_Init(LCD_DB03_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB04_PIN;
	GPIO_Init(LCD_DB04_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB05_PIN;
	GPIO_Init(LCD_DB05_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB06_PIN;
	GPIO_Init(LCD_DB06_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB07_PIN;
	GPIO_Init(LCD_DB07_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB08_PIN;
	GPIO_Init(LCD_DB08_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB09_PIN;
	GPIO_Init(LCD_DB09_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB10_PIN;
	GPIO_Init(LCD_DB10_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB11_PIN;
	GPIO_Init(LCD_DB11_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB12_PIN;
	GPIO_Init(LCD_DB12_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB13_PIN;
	GPIO_Init(LCD_DB13_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB14_PIN;
	GPIO_Init(LCD_DB14_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DB15_PIN;
	GPIO_Init(LCD_DB15_PORT, &GPIO_InitStruct);	
}

static void FSMC_Config(void)
{
	FSMC_NORSRAMTimingInitTypeDef FSMC_ReadTimingStruct;
	FSMC_NORSRAMTimingInitTypeDef FSMC_WriteTimingStruct;
	FSMC_NORSRAMInitTypeDef FSMC_NORInitStruct;
	
	RCC_AHBPeriphClockCmd(LCD_FSMC_CLK, ENABLE);
	
	FSMC_ReadTimingStruct.FSMC_AccessMode = FSMC_AccessMode_B;
	FSMC_ReadTimingStruct.FSMC_AddressHoldTime = 0x0;
	FSMC_ReadTimingStruct.FSMC_AddressSetupTime = 0x0;
	FSMC_ReadTimingStruct.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_ReadTimingStruct.FSMC_CLKDivision = 0;
	FSMC_ReadTimingStruct.FSMC_DataLatency = 0;
	FSMC_ReadTimingStruct.FSMC_DataSetupTime = 0x03;
	
	FSMC_WriteTimingStruct.FSMC_AccessMode = FSMC_AccessMode_B;
	FSMC_WriteTimingStruct.FSMC_AddressHoldTime = 0x0;
	FSMC_WriteTimingStruct.FSMC_AddressSetupTime = 0x0;
	FSMC_WriteTimingStruct.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_WriteTimingStruct.FSMC_CLKDivision = 0;
	FSMC_WriteTimingStruct.FSMC_DataLatency = 0;
	FSMC_WriteTimingStruct.FSMC_DataSetupTime = 0x01;
	
	FSMC_NORInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORInitStruct.FSMC_MemoryType = FSMC_MemoryType_NOR;
	FSMC_NORInitStruct.FSMC_ReadWriteTimingStruct = &FSMC_ReadTimingStruct;
	FSMC_NORInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORInitStruct.FSMC_WrapMode = FSMC_WrapMode_Enable;
	FSMC_NORInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORInitStruct.FSMC_WriteTimingStruct = &FSMC_WriteTimingStruct;
	
	FSMC_NORSRAMInit(&FSMC_NORInitStruct);
	FSMC_NORSRAMCmd(LCD_FSMC_BANK_NORSRAMx, ENABLE);
}

/**
  * @brief  向ILI9341写入命令
  * @param  usCmd :要写入的命令（表寄存器地址）
  * @retval 无
  */	
__inline void ILI9341_Write_Cmd ( uint16_t usCmd )
{
	*FSMC_Addr_ILI9341_CMD = usCmd;
}


/**
  * @brief  向ILI9341写入数据
  * @param  usData :要写入的数据
  * @retval 无
  */	
__inline void ILI9341_Write_Data ( uint16_t usData )
{
	*FSMC_Addr_ILI9341_DATA = usData;
}


/**
  * @brief  从ILI9341读取数据
  * @param  无
  * @retval 读取到的数据
  */	
__inline uint16_t ILI9341_Read_Data ( void )
{
	return *FSMC_Addr_ILI9341_DATA;
}


/**
  * @brief  用于 ILI9341 简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

/**
 * @brief  ILI9341 软件复位
 * @param  无
 * @retval 无
 */
void ILI9341_Rst( void )
{			
	GPIO_ResetBits ( LCD_RST_PORT, LCD_RST_PIN );	 //低电平复位
	ILI9341_Delay ( 0xAFF ); 					   
	GPIO_SetBits ( LCD_RST_PORT, LCD_RST_PIN );		 	 
	ILI9341_Delay ( 0xAFF ); 	
}

/**
 * @brief  ILI9341背光LED控制
 * @param  enumState ：决定是否使能背光LED
  *   该参数为以下值之一：
  *     @arg ENABLE :使能背光LED
  *     @arg DISABLE :禁用背光LED
 * @retval 无
 */
void ILI9341_BackLed_Control( FunctionalState enumState )
{
	if ( enumState )
		GPIO_ResetBits ( LCD_BL_PORT, LCD_BL_PIN );	
	else
		GPIO_SetBits ( LCD_BL_PORT, LCD_BL_PIN );
}


/**
 * @brief  读取LCD驱动芯片ID函数，可用于测试底层的读写函数
 * @param  无
 * @retval 正常时返回值为LCD驱动芯片ID: LCDID_ILI9341/LCDID_ST7789V
 *         否则返回: LCDID_UNKNOWN
 */
uint16_t ILI9341_ReadID(void)
{
	uint16_t id = 0;
	
	ILI9341_Write_Cmd(0x04);  //Read Display Identification Information
	ILI9341_Read_Data();
	ILI9341_Read_Data();
	id = ILI9341_Read_Data();
	id <<= 8;
	id |= ILI9341_Read_Data();
	
  if(id == LCDID_ST7789V)
  {
    return id;
  }
  else
  {
    ILI9341_Write_Cmd(0xD3);  //Read ID4
    ILI9341_Read_Data();
    ILI9341_Read_Data();
    id = ILI9341_Read_Data();
    id <<= 8;
    id |= ILI9341_Read_Data();
    if(id == LCDID_ILI9341)
    {
      return id;
    }
  }
  
	return LCDID_UNKNOWN;
}


 /**
 * @brief  初始化ILI9341寄存器
 * @param  无
 * @retval 无
 */
static void ILI9341_REG_Config ( void )
{
	//保存液晶屏驱动ic的 ID
	uint16_t lcdid = LCDID_UNKNOWN;
	
  lcdid = ILI9341_ReadID();
  
  if(lcdid == LCDID_ILI9341)
  {
    /*  Power control B (CFh)  */
    ILI9341_Write_Cmd ( 0xCF  );
    ILI9341_Write_Data ( 0x00  );
    ILI9341_Write_Data ( 0x81  );
    ILI9341_Write_Data ( 0x30  );
    
    /*  Power on sequence control (EDh) */
    ILI9341_Write_Cmd ( 0xED );
    ILI9341_Write_Data ( 0x64 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x12 );
    ILI9341_Write_Data ( 0x81 );
    
    /*  Driver timing control A (E8h) */
    ILI9341_Write_Cmd ( 0xE8 );
    ILI9341_Write_Data ( 0x85 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x78 );
    
    /*  Power control A (CBh) */
    ILI9341_Write_Cmd ( 0xCB );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x2C );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x34 );
    //ILI9341_Write_Data ( 0x02 );
    ILI9341_Write_Data ( 0x06 ); //原来是0x02改为0x06可防止液晶显示白屏时有条纹的情况
    
    /* Pump ratio control (F7h) */
    ILI9341_Write_Cmd ( 0xF7 );
    ILI9341_Write_Data ( 0x20 );
    
    /* Driver timing control B */
    ILI9341_Write_Cmd ( 0xEA );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
    ILI9341_Write_Cmd ( 0xB1 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x1B );
    
    /*  Display Function Control (B6h) */
    ILI9341_Write_Cmd ( 0xB6 );
    ILI9341_Write_Data ( 0x0A );
    ILI9341_Write_Data ( 0xA2 );
    
    /* Power Control 1 (C0h) */
    ILI9341_Write_Cmd ( 0xC0 );
    ILI9341_Write_Data ( 0x35 );
    
    /* Power Control 2 (C1h) */
    ILI9341_Write_Cmd ( 0xC1 );
    ILI9341_Write_Data ( 0x11 );
    
    /* VCOM Control 1 (C5h) */
    ILI9341_Write_Cmd ( 0xC5 );
    ILI9341_Write_Data ( 0x45 );
    ILI9341_Write_Data ( 0x45 );
    
    /*  VCOM Control 2 (C7h)  */
    ILI9341_Write_Cmd ( 0xC7 );
    ILI9341_Write_Data ( 0xA2 );
    
    /* Enable 3G (F2h) */
    ILI9341_Write_Cmd ( 0xF2 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Gamma Set (26h) */
    ILI9341_Write_Cmd ( 0x26 );
    ILI9341_Write_Data ( 0x01 );
    
    /* Positive Gamma Correction */
    ILI9341_Write_Cmd ( 0xE0 ); //Set Gamma
    ILI9341_Write_Data ( 0x0F );
    ILI9341_Write_Data ( 0x26 );
    ILI9341_Write_Data ( 0x24 );
    ILI9341_Write_Data ( 0x0B );
    ILI9341_Write_Data ( 0x0E );
    ILI9341_Write_Data ( 0x09 );
    ILI9341_Write_Data ( 0x54 );
    ILI9341_Write_Data ( 0xA8 );
    ILI9341_Write_Data ( 0x46 );
    ILI9341_Write_Data ( 0x0C );
    ILI9341_Write_Data ( 0x17 );
    ILI9341_Write_Data ( 0x09 );
    ILI9341_Write_Data ( 0x0F );
    ILI9341_Write_Data ( 0x07 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Negative Gamma Correction (E1h) */
    ILI9341_Write_Cmd ( 0XE1 ); //Set Gamma
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x19 );
    ILI9341_Write_Data ( 0x1B );
    ILI9341_Write_Data ( 0x04 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x07 );
    ILI9341_Write_Data ( 0x2A );
    ILI9341_Write_Data ( 0x47 );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x06 );
    ILI9341_Write_Data ( 0x06 );
    ILI9341_Write_Data ( 0x30 );
    ILI9341_Write_Data ( 0x38 );
    ILI9341_Write_Data ( 0x0F );
    
    /* memory access control set */
    ILI9341_Write_Cmd ( 0x36 ); 	
    ILI9341_Write_Data ( 0xC8 );    /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
    
    /* column address control set */
    ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );   //SC:0x0000-->0
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0xEF );   //EC:0x00EF-->239
    
    /* page address control set */
    ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );  //SP:0x0000-->0
    ILI9341_Write_Data ( 0x01 ); 
    ILI9341_Write_Data ( 0x3F );  //EP:013F-->319
    
    /*  Pixel Format Set (3Ah)  */
    ILI9341_Write_Cmd ( 0x3a ); 
    ILI9341_Write_Data ( 0x55 ); //16 bits/pixel
    
    /* Sleep Out (11h)  */
    ILI9341_Write_Cmd ( 0x11 );	
    ILI9341_Delay ( 0xAFFF<<2 );
    
    /* Display ON (29h) */
    ILI9341_Write_Cmd ( 0x29 ); 
	}
  
  else if(lcdid == LCDID_ST7789V)
  {
    /*  Power control B (CFh)  */
    ILI9341_Write_Cmd ( 0xCF  );
    ILI9341_Write_Data ( 0x00  );
    ILI9341_Write_Data ( 0xC1  );
    ILI9341_Write_Data ( 0x30  );
    
    /*  Power on sequence control (EDh) */
    ILI9341_Write_Cmd ( 0xED );
    ILI9341_Write_Data ( 0x64 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x12 );
    ILI9341_Write_Data ( 0x81 );
    
    /*  Driver timing control A (E8h) */
    ILI9341_Write_Cmd ( 0xE8 );
    ILI9341_Write_Data ( 0x85 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x78 );
    
    /*  Power control A (CBh) */
    ILI9341_Write_Cmd ( 0xCB );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x2C );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x34 );
    ILI9341_Write_Data ( 0x02 );
    
    /* Pump ratio control (F7h) */
    ILI9341_Write_Cmd ( 0xF7 );
    ILI9341_Write_Data ( 0x20 );
    
    /* Driver timing control B */
    ILI9341_Write_Cmd ( 0xEA );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    
    
    /* Power Control 1 (C0h) */
    ILI9341_Write_Cmd ( 0xC0 );   //Power control
    ILI9341_Write_Data ( 0x21 );  //VRH[5:0]
    
    /* Power Control 2 (C1h) */
    ILI9341_Write_Cmd ( 0xC1 );   //Power control
    ILI9341_Write_Data ( 0x11 );  //SAP[2:0];BT[3:0]
    
    /* VCOM Control 1 (C5h) */
    ILI9341_Write_Cmd ( 0xC5 );
    ILI9341_Write_Data ( 0x2D );
    ILI9341_Write_Data ( 0x33 );
    
    /*  VCOM Control 2 (C7h)  */
//	ILI9341_Write_Cmd ( 0xC7 );
//	ILI9341_Write_Data ( 0XC0 );
    
    /* memory access control set */
    ILI9341_Write_Cmd ( 0x36 );   //Memory Access Control
    ILI9341_Write_Data ( 0x00 );  /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
    
    ILI9341_Write_Cmd(0x3A);   
    ILI9341_Write_Data(0x55); 
    
      /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
    ILI9341_Write_Cmd ( 0xB1 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x17 );
    
    /*  Display Function Control (B6h) */
    ILI9341_Write_Cmd ( 0xB6 );
    ILI9341_Write_Data ( 0x0A );
    ILI9341_Write_Data ( 0xA2 );
    
    ILI9341_Write_Cmd(0xF6);    			
    ILI9341_Write_Data(0x01); 
    ILI9341_Write_Data(0x30); 
    
    /* Enable 3G (F2h) */
    ILI9341_Write_Cmd ( 0xF2 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Gamma Set (26h) */
    ILI9341_Write_Cmd ( 0x26 );
    ILI9341_Write_Data ( 0x01 );
    
    /* Positive Gamma Correction */
    ILI9341_Write_Cmd(0xe0); //Positive gamma
    ILI9341_Write_Data(0xd0);         
    ILI9341_Write_Data(0x00); 
    ILI9341_Write_Data(0x02); 
    ILI9341_Write_Data(0x07); 
    ILI9341_Write_Data(0x0b); 
    ILI9341_Write_Data(0x1a); 
    ILI9341_Write_Data(0x31); 
    ILI9341_Write_Data(0x54); 
    ILI9341_Write_Data(0x40); 
    ILI9341_Write_Data(0x29); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x12); 
    ILI9341_Write_Data(0x17);

    /* Negative Gamma Correction (E1h) */
    ILI9341_Write_Cmd(0xe1); //Negative gamma
    ILI9341_Write_Data(0xd0); 
    ILI9341_Write_Data(0x00); 
    ILI9341_Write_Data(0x02); 
    ILI9341_Write_Data(0x07); 
    ILI9341_Write_Data(0x05); 
    ILI9341_Write_Data(0x25); 
    ILI9341_Write_Data(0x2d); 
    ILI9341_Write_Data(0x44); 
    ILI9341_Write_Data(0x45); 
    ILI9341_Write_Data(0x1c); 
    ILI9341_Write_Data(0x18); 
    ILI9341_Write_Data(0x16); 
    ILI9341_Write_Data(0x1c); 
    ILI9341_Write_Data(0x1d); 
  
	
//	/* column address control set */
//	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0xEF );
//	
//	/* page address control set */
//	DEBUG_DELAY ();
//	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x00 );
//	ILI9341_Write_Data ( 0x01 );
//	ILI9341_Write_Data ( 0x3F );
	
	
    /* Sleep Out (11h)  */
    ILI9341_Write_Cmd ( 0x11 );	  //Exit Sleep
    ILI9341_Delay ( 0xAFFf<<2 );
    
    /* Display ON (29h) */
    ILI9341_Write_Cmd ( 0x29 );   //Display on
    
    ILI9341_Write_Cmd(0x2c);
  }
}


void LCD_FSMC_Init(void)
{
	LCD_GPIO_Config();
	FSMC_Config();
	ILI9341_BackLed_Control(ENABLE);
	ILI9341_Rst();
	ILI9341_REG_Config();
}


void drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	ILI9341_Write_Cmd(0x2A);
	ILI9341_Write_Data((x&0xFF00)>>8);
	ILI9341_Write_Data(x&0xFF);
	ILI9341_Write_Data((x&0xFF00)>>8);
	ILI9341_Write_Data(x&0xFF);
	
	ILI9341_Write_Cmd(0x2B);
	ILI9341_Write_Data((y&0xFF00)>>8);
	ILI9341_Write_Data(y&0xFF);
	ILI9341_Write_Data((y&0xFF00)>>8);
	ILI9341_Write_Data(y&0xFF);
	
	ILI9341_Write_Cmd(0x2C);
	ILI9341_Write_Data(color);
}

void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	uint16_t i = 0;
	ILI9341_Write_Cmd(CMD_SetCoordinateX);  //column
	ILI9341_Write_Data((x&0xFF00)>>8);
	ILI9341_Write_Data(x&0xFF);
	ILI9341_Write_Data(((width+x)&0xFF00)>>8);
	ILI9341_Write_Data((width+x)&0xFF);
	
	ILI9341_Write_Cmd(CMD_SetCoordinateY);  //row
	ILI9341_Write_Data((y&0xFF00)>>8);
	ILI9341_Write_Data(y&0xFF);
	ILI9341_Write_Data(((height+y)&0xFF00)>>8);
	ILI9341_Write_Data((height+y)&0xFF);
	
	ILI9341_Write_Cmd(CMD_SetPixel); //填充颜色
	for(i=0;i<width*height;i++)
	{
		ILI9341_Write_Data(color);
	}
}


uint16_t ILI9341_ReadPixelFormat(void)
{
	ILI9341_Write_Cmd(0x0C);
	ILI9341_Read_Data();
	return ILI9341_Read_Data();
}



