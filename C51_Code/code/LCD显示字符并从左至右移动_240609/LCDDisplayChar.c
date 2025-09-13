#include <reg51.h>

//往LCD液晶显示中写字符或者字符串，以及移动字符串

sbit LCD_WR = P2^5;
sbit LCD_RS = P2^6;
sbit LCD_EN = P2^7;

#define LCD_DB P0

void delay_N_40us(int delayTime);
void LCDInit();  //初始化LCD
char readLCDState();  //读取LCD的状态
void writeLCDCommand(unsigned char cmd);  //向LCD写指令
char readLCDData();  //读取LCD的数据
void writeLCDData(unsigned char dat);  //向LCD写数据
void LCDDisplayChar(unsigned char x, unsigned char y, unsigned char dat);  //向LCD写一个字符
void LCDDisplatString(unsigned char x, unsigned char y, char *str);  //向LCD写字符串，第一行显示不完的字符会放在第二行继续显示
void LCDDisplatStringOverflow(unsigned char x, unsigned char y, char *str); //向LCD写字符串，第一行显示不完的字符会放在第一行后面不显示的位置
void LCDClear();

int main()
{
	LCDInit();
	LCDDisplatStringOverflow(20,1,"welcome to CQ");
	LCDDisplatStringOverflow(20,2,"hello world");
	
	while(1)
	{
		writeLCDCommand(0x18);
		delay_N_40us(10000);
	}
	return 0;
}

void LCDInit()
{
	//while((readLCDState() & 0x80) != 0);  //检查液晶显示屏是否处于忙的状态 P0^7 = 1忙，P0^7 = 0空闲,检查是否处于忙的状态成功率很低

	writeLCDCommand(0x38);  //设置数据总线为8位，显示2行，每行的字符大小为5*7
	writeLCDCommand(0x06);  //写入新数据光标右移，显示屏不移动	
	writeLCDCommand(0x0E);  //显示功能打开，有光标，光标不闪烁
	writeLCDCommand(0x14);  //光标右移一格，且AC值加1
	writeLCDCommand(0x01);  //清屏
	
	delay_N_40us(100);
}

char readLCDState()
{
	LCD_RS = 0;
	LCD_WR = 1;
	LCD_EN = 1;
	delay_N_40us(1);
	return LCD_DB;
}

void writeLCDCommand(unsigned char cmd)
{
	LCD_RS = 0;
	LCD_WR = 0;
	LCD_EN = 1;
	LCD_DB = cmd;
	delay_N_40us(1);
	LCD_EN = 0;
	delay_N_40us(1);
}

char readLCDData()
{
	LCD_RS = 1;
	LCD_WR = 1;
	LCD_EN = 1;
	delay_N_40us(1);
	return LCD_DB;
}

void writeLCDData(unsigned char dat)
{
	LCD_RS = 1;
	LCD_WR = 0;
	LCD_EN = 1;
	LCD_DB = dat;
	delay_N_40us(1);
	LCD_EN = 0;
	delay_N_40us(1);
}

void LCDDisplayChar(unsigned char x, unsigned char y, unsigned char dat)
{
	if(y == 1)
		writeLCDCommand(0x80 + (x-1));
	else
		writeLCDCommand(0xC0 + (x-1));
	
	writeLCDData(dat);
}

void LCDDisplatStringOverflow(unsigned char x, unsigned char y, char *str)
{
	if(y == 1)
		writeLCDCommand(0x80 + (x-1));
	else
		writeLCDCommand(0xC0 + (x-1));
	
	while(*str != '\0')
	{
		writeLCDData(*str);
		str++;
	}
}

void LCDDisplatString(unsigned char x, unsigned char y, char *str)
{
	unsigned char address = 0;
	if(y == 1)
		address = 0x80 + (x-1);
	else
		address = 0xC0 + (x-1);
	
	while(*str != '\0')
	{
		writeLCDCommand(address);
		writeLCDData(*str);
		str++;
		address++;
		if(((address & 0x7F) > 0x0F) && (((address & 0x7F) < 0x27)))
		{
			address = 0xC0;
		}
		else if(((address & 0x7F) > 0x4F) && ((address & 0x7F) < 0x67))
		{
			address = 0x80;
		}
	}
}

void LCDClear()
{
	writeLCDCommand(0x01);  //清屏
}

void delay_N_40us(int delayTime)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < delayTime;i++)
		for(j = 0;j < 5;j++ );
}


	