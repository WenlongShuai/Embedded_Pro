#include "oled.h"
#include "oled_codetab.h"
#include <string.h>
#include "delay.h"

OLED_MENU_STRUCT oled_menu_struct;
extern uint8_t keyStateFlag;

uint8_t OLED_Buffer[OLED_PAGES][OLED_WIDTH];  // 128×8字节缓冲区

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;

static void IIC_Write_Byte(uint8_t addr, uint8_t data)
{
//	while (I2C_GetFlagStatus(IIC2_I2Cx, I2C_FLAG_BUSY));

//	I2C_GenerateSTART(IIC2_I2Cx, ENABLE); /* IIC_Start信号 */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /*EV5,主模式*/

//	I2C_Send7bitAddress(IIC2_I2Cx, OLED_ID, I2C_Direction_Transmitter);  /* 呼叫从机 */
//	delay_ms(20);
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

//	I2C_SendData(IIC2_I2Cx, addr);  /* 配置数据写入地址 */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_SendData(IIC2_I2Cx, data);  /* 发送数据 */
//	while (!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_GenerateSTOP(IIC2_I2Cx, ENABLE); /* IIC_Stop信号 */
	
	
	/* Send STRAT condition */
  I2C_GenerateSTART(IIC2_I2Cx, ENABLE);
	I2CTimeout = I2CT_FLAG_TIMEOUT; 
	
	/* Test on EV5 and clear it */
  while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return;
  } 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send address for write */
  I2C_Send7bitAddress(IIC2_I2Cx, OLED_ID, I2C_Direction_Transmitter);
	/* Test on EV6 and clear it */
  while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return;
  }  
	
	/* Send internal address to write to */
	I2C_SendData(IIC2_I2Cx, addr);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	} 
	
	/* Send the byte to be written */
	I2C_SendData(IIC2_I2Cx, data); 
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(IIC2_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return;
	} 
  
  /* Send STOP condition */
  I2C_GenerateSTOP(IIC2_I2Cx, ENABLE);
}

/* oled写数据 */
void Oled_Write_Data(uint8_t data)
{
#if IIC_SELECT
	IIC_Write_Byte(OLED_WR_DATA,data);
#else
	IIC2_Start();
	IIC2_Send_Byte(OLED_ID);
	/* 等待应答 */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(OLED_WR_DATA);
	/* 等待应答 */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(data);
	/* 等待应答 */
	while (IIC2_Wait_Ack());
	
	IIC2_Stop();
#endif
}

/* oled写命令 */
void Oled_Write_Cmd(uint8_t cmd)
{
	#if IIC_SELECT
	IIC_Write_Byte(OLED_WR_CMD,cmd);
	#else
	IIC2_Start();
	IIC2_Send_Byte(OLED_ID);
	/* 等待应答 */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(OLED_WR_CMD);
	/* 等待应答 */
	while (IIC2_Wait_Ack());

	IIC2_Send_Byte(cmd);
	/* 等待应答 */
	while (IIC2_Wait_Ack());

	IIC2_Stop();
#endif
}

void OLED_Init(void)
{
	/* 设置显示打开/关闭
	 * AE--->显示打开
	 * AF--->显示关闭(休眠模式)
	 */
	Oled_Write_Cmd(0xAE);

	/* ================== 基本命令表 ===================*/
	/* 设置对比度
	 * 0~255：数值越大 亮度越亮
	 */
	Oled_Write_Cmd(0x81);
	Oled_Write_Cmd(0xFF);

	/* 使能全屏显示
	 * A4--->恢复到RAM内容显示
	 * A5--->忽略RAM内容显示
	 */
	Oled_Write_Cmd(0xA4);

	/* 设置显示模式
	 * A6--->正常显示：0灭1亮
	 * A7--->逆显示：1灭0亮
	 */
	Oled_Write_Cmd(0xA6);

	/* ================== 滚动命令表 ===================*/
	/* 滚动使能/失能
	 * 2E--->失能
	 * 2F--->使能
	 */
	Oled_Write_Cmd(0x2E);

	/* 七字节命令： 连续水平滚动设置 */

	/* 左/右水平滚动设置
	 * 26--->右水平滚动
	 * 27--->左水平滚动
	 */
	Oled_Write_Cmd(0x26);
	/* 虚拟字节 */
	Oled_Write_Cmd(0x00);
	/* 设置滚动起始页地址 */
	Oled_Write_Cmd(0x00);
	/* 设置滚动间隔 */
	Oled_Write_Cmd(0x03);
	/* 设置滚动结束地址 */
	Oled_Write_Cmd(0x07);
	/* 虚拟字节 */
	Oled_Write_Cmd(0x00);
	Oled_Write_Cmd(0xFF);

	/* =============== 寻址设置命令表 ==================*/

	/* 双字节命令:寄存器寻址模式 */
	Oled_Write_Cmd(0x20);

	/* 10:页寻址模式
	 * 01:垂直寻址模式
	 * 00:水平寻址模式
	 */
	Oled_Write_Cmd(0x10);
	/* 单字节命令:设置页寻址的起始页地址 */
	Oled_Write_Cmd(0xB0);
	/* 单字节命令:设置页寻址的起始列地址低位 */
	Oled_Write_Cmd(0x00);
	/* 单字节命令:设置页寻址的起始列地址高位 */
	Oled_Write_Cmd(0x10);

	/*=============== 硬件配置命令表 ==================*/

	/* 设置显示开始线
	 * 0x40~0x7F对应0~63
	 */
	Oled_Write_Cmd(0x40);

	/* 设置列重映射
	 * A0:addressX--->segX
	 * A1:addressX--->seg(127-X)
	 */
	Oled_Write_Cmd(0xA1);

	/* 设置多路复用比 */
	Oled_Write_Cmd(0xA8);
	Oled_Write_Cmd(0x3F);

	/* 设置COM输出扫描方向
	 * C0：COM0--->COM63(从上往下扫描)
	 * C8：COM63--->COM0(从下往上扫描)
	 */
	Oled_Write_Cmd(0xC8);

	/* 双字节命令：设置COM显示偏移量 */
	Oled_Write_Cmd(0xD3);
	Oled_Write_Cmd(0x00); /* COM不偏移 */

	/* 双字节命令：配置COM重映射 */
	Oled_Write_Cmd(0xDA);
	Oled_Write_Cmd(0x12);

	/* 双字节命令：设置预充期 */
	Oled_Write_Cmd(0xD9);
	Oled_Write_Cmd(0x22); /* 阶段一2个无效DCLK时钟/阶段二2个无效DCLK时钟 */

	/* 设置VCOMH取消选择电平
	 * 00:0.65xVcc
	 * 20:0.77xVcc
	 * 30:0.83xVcc
	 */
	Oled_Write_Cmd(0xDB);
	Oled_Write_Cmd(0x20);

	/* 双字节命令：设置电荷泵 */
	Oled_Write_Cmd(0x8d);
	Oled_Write_Cmd(0x14);

	Oled_Write_Cmd(0xAF);
	
	OLED_CLS();                                              // 清屏
	
	for(int i = 0;i < 3;i++)
	{
		for(int j = 0;j < 3;j++)
			oled_menu_struct.chooseNum[i][j] = 0;
	}
	oled_menu_struct.chooseNum[0][0] = 1;
}

/**
 * @brief  设置光标
 * @param  x,光标x位置
 *		   y,光标y位置
 * @retval 无
 */
void OLED_SetPos(unsigned char x, unsigned char y) // 设置起始点坐标
{
	Oled_Write_Cmd(0xb0 + y);
	Oled_Write_Cmd(((x & 0xf0) >> 4) | 0x10);
	Oled_Write_Cmd((x & 0x0f) | 0x01);
}

/**
 * @brief  填充整个屏幕
 * @param  fill_data:要填充的数据
 * @retval 无
 */
void OLED_Fill(unsigned char fill_data) // 全屏填充
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		Oled_Write_Cmd(0xb0 + m); // page0-page1
		Oled_Write_Cmd(0x00);     // low column start address
		Oled_Write_Cmd(0x10);     // high column start address
		for (n = 0; n < 128; n++)
		{
			Oled_Write_Data(fill_data);
		}
	}
}

/**
 * @brief  反相指定区域
 * @param  startX, startY : 起始点坐标(startX:0~127, startY:0~63)
						width, height : 填充的宽度和高度(width:0~127, height:0~63)
 * @retval 无
 */
void OLED_Appoint(unsigned char startX, unsigned char startY,unsigned char width, unsigned char height) // 指定位置填充
{
	for (uint16_t i = startX; i < startX + width && i < OLED_WIDTH; i++) 
	{
		for (uint16_t j = startY; j < startY + height && j < OLED_HEIGHT; j++) 
		{
			uint8_t page = j / 8;
			uint8_t bit = j % 8;
			OLED_Buffer[page][i] ^= (1 << bit);  // 异或操作反转位
		}
	}
	OLED_UpdateScreen(startX, startY, width, height);
}

// 更新显示（将缓冲区内容发送到OLED）
void OLED_UpdateScreen(uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
//	for (uint8_t page = 0; page < OLED_PAGES; page++) 
//	{
//		// 设置页地址
//		Oled_Write_Cmd(0xB0 + page);  // 页0-7
//		Oled_Write_Cmd(0x00);         // 列低地址
//		Oled_Write_Cmd(0x10);         // 列高地址
//		
//		// 发送一行数据
//		uint8_t data[OLED_WIDTH];
//		memcpy(&data[0], OLED_Buffer[page], OLED_WIDTH);
//		
//		
//		for(uint8_t i = 0;i < OLED_WIDTH;i++)
//		{
//			Oled_Write_Data(data[i]);
//		}
//	}
	
	uint8_t start_page = y / 8;
	uint8_t end_page = (y + h - 1) / 8;
	
	for (uint8_t page = start_page; page <= end_page; page++) 
	{
		// 设置页地址
		Oled_Write_Cmd(0xB0 + page);
		Oled_Write_Cmd(0x00 + (x & 0x0F));     // 列低地址
		Oled_Write_Cmd(0x10 + ((x >> 4) & 0x0F)); // 列高地址
		
		// 发送该页指定区域的数据
		uint8_t data[w];
		memcpy(&data[0], &OLED_Buffer[page][x], w);
		
		for(uint8_t i = 0;i < w;i++)
		{
			Oled_Write_Data(data[i]);
		}
	}
}

/**
 * @brief  清屏
 * @param  无
 * @retval 无
 */
void OLED_CLS(void) // 清屏
{
	OLED_Fill(0x00);
}

/**
 * @brief  将OLED从休眠中唤醒
 * @param  无
 * @retval 无
 */
void OLED_ON(void)
{
	Oled_Write_Cmd(0x8D); // 设置电荷泵
	Oled_Write_Cmd(0x14); // 开启电荷泵
	Oled_Write_Cmd(0xAF); // OLED唤醒
}

/**
 * @brief  让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
 * @param  无
 * @retval 无
 */
void OLED_OFF(void)
{
	Oled_Write_Cmd(0x8D); // 设置电荷泵
	Oled_Write_Cmd(0x10); // 关闭电荷泵
	Oled_Write_Cmd(0xAE); // OLED休眠
}


/**
 * @brief 显示codetab.h中的ASCII字符,有6*8和8*16可选择
 * @param  x,y : 起始点坐标(x:0~127, y:0~7);
 *					ch[] :- 要显示的字符串;
 *					textsize : 字符大小(1:6*8 ; 2:8*16)
 * @retval 无
 */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char textsize)
{
	unsigned char c = 0, i = 0, j = 0;
	switch (textsize)
	{
	case 1:
	{
		while (ch[j] != '\0')
		{
				c = ch[j] - 32;
				if (x > 126)
				{
						x = 0;
						y++;
				}
				OLED_SetPos(x, y);
				for (i = 0; i < 6; i++)
						Oled_Write_Data(F6x8[c][i]);
				x += 6;
				j++;
		}
	}
	break;
	case 2:
	{
		while (ch[j] != '\0')
		{
				c = ch[j] - 32;
				if (x > 120)
				{
						x = 0;
						y++;
				}
				OLED_SetPos(x, y);
				for (i = 0; i < 8; i++)
						Oled_Write_Data(F8X16[c * 16 + i]);
				OLED_SetPos(x, y + 1);
				for (i = 0; i < 8; i++)
						Oled_Write_Data(F8X16[c * 16 + i + 8]);
				x += 8;
				j++;
		}
	}
	break;
	}
}

/**
 * @brief  OLED_ShowCN，显示codetab.h中的汉字,16*16点阵
 * @param  x,y: 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
 *
 * @retval 无
 */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n)
{
	unsigned char wm = 0;
	unsigned int adder = 32 * n;
	OLED_SetPos(x, y);
	for (wm = 0; wm < 16; wm++)
	{
			Oled_Write_Data(F16x16[adder]);
			adder += 1;
	}
	OLED_SetPos(x, y + 1);
	for (wm = 0; wm < 16; wm++)
	{
			Oled_Write_Data(F16x16[adder]);
			adder += 1;
	}
}

/**
 * @brief  OLED_DrawBMP，显示BMP位图
 * @param  x0,y0 :起始点坐标(x0:0~127, y0:0~7);x1,y1 : 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
 * @retval 无
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
	{
			y = y1 / 8;
	}
	else
	{
			y = y1 / 8 + 1;
	}
	for (y = y0; y < y1; y++)
	{
			OLED_SetPos(x0, y);
			for (x = x0; x < x1; x++)
			{
					Oled_Write_Data(BMP[j++]);
			}
	}
}




void setOLEDDisplay(KeyEvent_t keyEvent)
{
	switch(keyEvent)
	{
		case KEY_EVENT_CLICK:  //按键单击
			
			break;
		case KEY_EVENT_DOUBLE_CLICK:  //按键双击
			
			break;
	
	}
	
	
	/* 主界面 */
	#if 1
	OLED_DrawBMP(0, 0, 16, 2, (unsigned char *)bluetoothIcon);   //蓝牙图标
	OLED_DrawBMP(20, 0, 36, 2, (unsigned char *)wifiIcon);		//WiFi图标
	OLED_DrawBMP(111, 0, 127, 2, (unsigned char *)electricQuantityIco3);       //电池电量图标
	OLED_ShowStr(44,2,(unsigned char *)"17:58",2);
	OLED_ShowStr(50,4,(unsigned char *)"2025",1);
	
//	OLED_Appoint(96,48,32,16);
	
	for (uint8_t i = 0; i < 2; i++)
	{
			OLED_ShowCN(0+ i * 16, 6, i); // 测试显示中文
	}
	
	for (uint8_t i = 0; i < 2; i++)
	{
			OLED_ShowCN(96+ i * 16, 6, i+2); // 测试显示中文
	}
	
	
	#endif
	
	#if 0
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	#endif
	
//	delay_ms(5000);
}

void oled_Main_Menu_Display(void)
{
	/* 主界面 */
	OLED_CLS();
	OLED_DrawBMP(0, 0, 16, 2, (unsigned char *)bluetoothIcon);   //蓝牙图标
	OLED_DrawBMP(20, 0, 36, 2, (unsigned char *)wifiIcon);		//WiFi图标
	OLED_DrawBMP(111, 0, 127, 2, (unsigned char *)electricQuantityIco3);   //电池电量图标
	OLED_ShowStr(44,2,(unsigned char *)"17:58",2);
	OLED_ShowStr(50,4,(unsigned char *)"2025",1);
		
	for (uint8_t i = 0; i < 2; i++)
	{
		OLED_ShowCN(0+ i * 16, 6, 6+i); //“设置”
	}
	
	for (uint8_t i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 18+i); //“显示”
	}
}

void oled_Set_Menu_Display(void)
{
	uint8_t i;
	OLED_CLS();
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 0, 0+i); //“时间”
		else
			OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //“设置”
	}
	OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16字符
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 2, 2+i); //“蓝牙”
		else
			OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //“设置”
	}
	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16字符
	
	for (i = 0; i < 4; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 4, 4+i); //“无线设置”
		else
			OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //“无线设置”
	}
	OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16字符

	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void oled_Dis_Menu_Display(void)
{
	uint8_t i;
	OLED_CLS();
	
	for (i = 0; i < 5; i++)
	{
		if(i < 3)
			OLED_ShowCN(0+ i * 16, 0, 8+i); //“电机左”
		else
			OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //“显示”
	}
	OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
	
	for (i = 0; i < 5; i++)
	{
		if(i < 2)
			OLED_ShowCN(0+ i * 16, 2, 8+i); //“电机”
		else if(i == 2)
			OLED_ShowCN(0+ i * 16, 2, 11); //“右”
		else
			OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //“显示”
	}
	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
	
	OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(24+ i * 16, 4, 18+i); //“显示”
	}
	OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
	
//	OLED_ShowStr(0, 6, (unsigned char *)"IMU",2);
//	for (i = 0; i < 4; i++)
//	{
//		OLED_ShowCN(24+ i * 16, 6, 18+(i-3)); //“显示”
//	}
//	OLED_ShowStr(24 + i * 16, 6, (unsigned char *)" >",2);
//	
//	for (i = 0; i < 6; i++)
//	{
//		if(i < 4)
//			OLED_ShowCN(0+ i * 16, 6, 12+i); //“控制模式”
//		else
//			OLED_ShowCN(0+ i * 16, 6, 18+(i-4)); //“显示”
//	}
//	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
//	
//	for (i = 0; i < 4; i++)
//	{
//		if(i < 2)
//			OLED_ShowCN(0+ i * 16, 6, i); //“全部”
//		else
//			OLED_ShowCN(0+ i * 16, 6, 18+(i-3)); //“显示”
//	}
//	OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);

	for (i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void main_Menu_Operate(void)
{
	keyStateFlag = 0;
	switch(oled_menu_struct.chooseNum[0][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (uint8_t i = 0; i < 2; i++)
				{
					OLED_ShowCN(0+ i * 16, 6, 6+i); //“设置”
				}
				delay_ms(100);
				OLED_ShowStr(0, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (uint8_t i = 0; i < 2; i++)
			{
				OLED_ShowCN(0+ i * 16, 6, 6+i); //“设置”
			}
			break;
		case 2:
			while(1)
			{
				for (uint8_t i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 18+i); //“显示”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (uint8_t i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 18+i); //“显示”
			}
			break;
	}
}

void set_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[1][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 0, 0+i); //“时间”
					else
						OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //“设置”
				}
				OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16字符
				
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 0, 0+i); //“时间”
				else
					OLED_ShowCN(0+ i * 16, 0, 6+(i-2)); //“设置”
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);   //8*16字符
			break;
		case 2:
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 2, 2+i); //“蓝牙”
					else
						OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //“设置”
				}
				OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16字符
				delay_ms(100);
				OLED_ShowStr(0, 2, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 2+i); //“蓝牙”
				else
					OLED_ShowCN(0+ i * 16, 2, 6+(i-2)); //“设置”
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);   //8*16字符
			break;
		case 3:
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 4, 4+i); //“无线设置”
					else
						OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //“无线设置”
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16字符
				delay_ms(100);
				
				OLED_ShowStr(0, 4, (unsigned char *)"          ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 4; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 4, 4+i); //“无线设置”
				else
					OLED_ShowCN(0+ i * 16, 4, 6+(i-2)); //“无线设置”
			}
			OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);   //8*16字符
			break;
		case 4:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void set_Time_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void set_Bluetooth_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void set_WIFI_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][2])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void dis_Menu_Operate(void)
{
	keyStateFlag = 0;
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 4, 18+i); //“显示”
			}
			OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
			
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 8+i); //“电机”
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 2, 11); //“右”
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 5; i++)
				{
					if(i < 3)
						OLED_ShowCN(0+ i * 16, 0, 8+i); //“电机左”
					else
						OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //“显示”
				}
				OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 2:
			for (i = 0; i < 5; i++)
			{
				if(i < 3)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //“电机左”
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 4, 18+i); //“显示”
			}
			OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 5; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 2, 8+i); //“电机”
					else if(i == 2)
						OLED_ShowCN(0+ i * 16, 2, 11); //“右”
					else
						OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //“显示”
				}
				OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 3:
			for (i = 0; i < 5; i++)
			{
				if(i < 3)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //“电机左”
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
				
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 2, 8+i); //“电机”
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 2, 11); //“右”
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				OLED_ShowStr(0, 4, (unsigned char *)"GPS",2);
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(24+ i * 16, 4, 18+i); //“显示”
				}
				OLED_ShowStr(24+i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 0, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 4:
			for (i = 0; i < 5; i++)
			{
				if(i < 2)
					OLED_ShowCN(0+ i * 16, 0, 8+i); //“电机”
				else if(i == 2)
					OLED_ShowCN(0+ i * 16, 0, 11); //“右”
				else
					OLED_ShowCN(0+ i * 16, 0, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 2, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 2, 18+i); //“显示”
			}
			OLED_ShowStr(24+i * 16, 2, (unsigned char *)" >",2);
		
			while(1)
			{
				OLED_ShowStr(0, 4, (unsigned char *)"IMU",2);
				for (i = 0; i < 4; i++)
				{
					OLED_ShowCN(24+ i * 16, 4, 18+(i-3)); //“显示”
				}
				OLED_ShowStr(24 + i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 5:
			OLED_ShowStr(0, 0, (unsigned char *)"GPS",2);
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(24+ i * 16, 0, 18+i); //“显示”
			}
			OLED_ShowStr(24+i * 16, 0, (unsigned char *)" >",2);
			
			OLED_ShowStr(0, 2, (unsigned char *)"IMU",2);
			for (i = 0; i < 4; i++)
			{
				OLED_ShowCN(24+ i * 16, 2, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(24 + i * 16, 2, (unsigned char *)" >",2);
			
		
			while(1)
			{
				for (i = 0; i < 6; i++)
				{
					if(i < 4)
						OLED_ShowCN(0+ i * 16, 4, 12+i); //“控制模式”
					else
						OLED_ShowCN(0+ i * 16, 4, 18+(i-4)); //“显示”
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 6:
			OLED_ShowStr(0, 0, (unsigned char *)"IMU",2);
			for (i = 0; i < 4; i++)
			{
				OLED_ShowCN(24+ i * 16, 0, 18+(i-3)); //“显示”
			}
			OLED_ShowStr(24 + i * 16, 0, (unsigned char *)" >",2);
			
			for (i = 0; i < 6; i++)
			{
				if(i < 4)
					OLED_ShowCN(0+ i * 16, 2, 12+i); //“控制模式”
				else
					OLED_ShowCN(0+ i * 16, 2, 18+(i-4)); //“显示”
			}
			OLED_ShowStr(i * 16, 2, (unsigned char *)" >",2);
			while(1)
			{
				for (i = 0; i < 4; i++)
				{
					if(i < 2)
						OLED_ShowCN(0+ i * 16, 4, i); //“全部”
					else
						OLED_ShowCN(0+ i * 16, 4, 18+(i-3)); //“显示”
				}
				OLED_ShowStr(i * 16, 4, (unsigned char *)" >",2);
				delay_ms(100);
				OLED_ShowStr(0, 4, (unsigned char *)"            ",2);
				if(keyStateFlag)
					break;
			}
			break;
		case 7:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}

}

void dis_MotorLeft_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void dis_MotorRight_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}

}

void dis_GPS_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}
}

void dis_IMU_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}

}

void dis_ControlMode_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}

}

void dis_All_Menu_Operate(void)
{
	int i = 0;
	switch(oled_menu_struct.chooseNum[2][1])
	{
		case 0:
			while(1)
			{
				delay_ms(100);
				if(keyStateFlag)
					break;
			}
			break;
		case 1:
			while(1)
			{
				for (i = 0; i < 2; i++)
				{
					OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
				}
				delay_ms(100);
				OLED_ShowStr(96, 6, (unsigned char *)"    ",2);
				if(keyStateFlag)
					break;
			}
			for (i = 0; i < 2; i++)
			{
				OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
			}
			break;
	}

}

void keyClickOLEDDis(void (*clickOperate)(void))
{
	clickOperate();
	
	
	
	
	
	
//	switch(oled_menu_struct.menuNum)
//	{
//		case 0:  //主界面
//			
//			break;
//		case 1:	 //设置界面
//			break; 
//		case 2:	 //显示界面
//			break;
//	}
	

}

void keyDoubleClickOLEDDis(void (*doubleClickOperate)(void))
{
	doubleClickOperate();
}


void setTimeMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"TIME",2);   //8*16字符
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void setBluetoothMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"BLUETOOTH",2);   //8*16字符

	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void setWIFIMenuDisplay(void)
{
	OLED_CLS();
	OLED_ShowStr(48, 4, (unsigned char *)"WIFI",2);   //8*16字符

	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void disMotorLeftDisplay(void)
{
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"+ 100",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void disMotorRightDisplay(void)
{
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"+ 100",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void disGPSDisplay(void)
{
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void disIMUDisplay(void)
{
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}

void disControlModeDisplay(void)
{

}

void disALLDisplay(void)
{
	OLED_ShowStr(0,0,(unsigned char *)"L:+ 100",1);
	OLED_ShowStr(64,0,(unsigned char *)"+ 100",1);
	OLED_ShowStr(0,1,(unsigned char *)"R:+ 100",1);
	OLED_ShowStr(64,1,(unsigned char *)"+ 100",1);
	OLED_ShowStr(0,2,(unsigned char *)"GPS:",1);
	OLED_ShowStr(32,2,(unsigned char *)"119.00000001",1);
	OLED_ShowStr(32,3,(unsigned char *)"39.00000001",1);
	OLED_ShowStr(32,4,(unsigned char *)"1000",1);
	OLED_ShowStr(0,5,(unsigned char *)"IMU:",1);
	OLED_ShowStr(32,5,(unsigned char *)"111",1);
	OLED_ShowStr(32,6,(unsigned char *)"222",1);
	OLED_ShowStr(32,7,(unsigned char *)"333",1);
	
	for (int i = 0; i < 2; i++)
	{
		OLED_ShowCN(96+ i * 16, 6, 24+i); //“退出”
	}
}
