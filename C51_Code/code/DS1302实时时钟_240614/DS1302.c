#include "DS1302.h"

sbit DS1302_SCLK = P3^6;
sbit DS1302_IO = P3^4;
sbit DS1302_CE = P3^5;

//DS1302写入时钟和日历的地址命令
uchar code ds1302WriteAddr[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C};

//DS1302读取时钟和日历的地址命令
uchar code ds1302ReadAddr[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8B, 0x8D};

//DS1302初始化时间为2024年06月15日星期六1点2分0秒
uchar code ds1302Time[7] = {0x00, 0x02, 0x01, 0x15, 0x06, 0x06, 0x24};

//读取的时间
uchar ds1302Read[7] = {0};

void DS1302DelayUs(uint);

/*******************************************************************************
* 函 数 名       : DS1302Init
* 函数功能		 	 : DS1302初始化，将初始的时钟日历写入DS1302的寄存器中
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void DS1302Init()
{
	uchar i = 0;
	DS1302WriteByte(0x8E, 0x00);  //关闭写保护，将0x8E寄存器的WP为写0
	
	//初始化DS1302的时钟和日历
	for(i = 0;i < 7;i++)
	{
		DS1302WriteByte(ds1302WriteAddr[i], ds1302Time[i]);
	}
}

void DS1302AdjustmentTime(int *dat)
{
	uchar i = 0;
	uchar tmp = 0;
	DS1302WriteByte(0x8E, 0x00);  //关闭写保护，将0x8E寄存器的WP为写0
	
	//初始化DS1302的时钟和日历
	for(i = 0;i < 7;i++)
	{
		tmp = ((dat[i] % 10) & 0x0f) | (((dat[i] / 10) << 4) & 0xf0);
		DS1302WriteByte(ds1302WriteAddr[i], tmp);
	}
}

/*******************************************************************************
* 函 数 名       : DS1302WriteByte
* 函数功能		 	 : DS1302单字节写入
* 输    入       : writeAddr:寄存器的写地址（地址/命令）
										dat:写入寄存器中的值
* 输    出    	 : 无
*******************************************************************************/
void DS1302WriteByte(uchar writeAddr, uchar dat)
{
	uchar i = 0;
	DS1302_CE = 0;
	DS1302_SCLK = 0;
	DS1302_CE = 1;
	DS1302DelayUs(1);
	for(i = 0;i < 8;i++)
	{
		DS1302_SCLK = 0;
		DS1302DelayUs(1);
		
		DS1302_IO = (writeAddr >> i) & 1;
		
		DS1302_SCLK = 1;
		DS1302DelayUs(1);
	}
	for(i = 0;i < 8;i++)
	{
		DS1302_SCLK = 0;
		DS1302DelayUs(1);
		DS1302_IO = (dat >> i) & 1;
		DS1302_SCLK = 1;
		DS1302DelayUs(1);
	}
	DS1302_CE = 0;
	DS1302_SCLK = 0;
	DS1302DelayUs(1);
}

/*******************************************************************************
* 函 数 名       : DS1302ReadByte
* 函数功能		 	 : DS1302单字节读出
* 输    入       : readAddr:寄存器的读地址（地址/命令）
* 输    出    	 : read:读取的数据
*******************************************************************************/
uchar DS1302ReadByte(uchar readAddr)
{
	uchar i = 0;
	uchar read = 0;
	uchar tmp = 0;
	DS1302_CE = 0;
	DS1302_SCLK = 0;
	DS1302_CE = 1;
	DS1302DelayUs(1);
	for(i = 0;i < 8;i++)
	{
		DS1302_SCLK = 0;
		DS1302DelayUs(1);
		DS1302_IO = (readAddr >> i) & 1;
		DS1302_SCLK = 1;
		DS1302DelayUs(1);
	}
	DS1302DelayUs(2);
	for(i = 0;i < 8;i++)
	{
		DS1302_SCLK = 0;
		DS1302DelayUs(1);
		tmp = DS1302_IO;
		DS1302_SCLK = 1;
		DS1302DelayUs(1);
		read |= tmp << i;
	}
	DS1302_CE = 0;
	DS1302DelayUs(1);
	DS1302_IO = 0;
	DS1302DelayUs(1);
	DS1302_IO  = 1;
	DS1302DelayUs(1);
	return read;
}

/*******************************************************************************
* 函 数 名       : readTime
* 函数功能		 	 : 从DS1302寄存器中读取时钟日历
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void readTime()
{
	uchar i = 0;
	for(i = 0;i < 7;i++)
	{
		ds1302Read[i] = DS1302ReadByte(ds1302ReadAddr[i]);
	}
}

/*******************************************************************************
* 函 数 名       : DS1302DelayUs
* 函数功能		 		 : 延时函数，us = 1，延时3us
* 输    入       : us:延时时间
* 输    出    	 : 无
*******************************************************************************/
void DS1302DelayUs(uint us)
{
	while(us--);
}

