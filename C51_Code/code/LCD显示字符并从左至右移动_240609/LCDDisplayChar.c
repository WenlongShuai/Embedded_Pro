#include <reg51.h>

//��LCDҺ����ʾ��д�ַ������ַ������Լ��ƶ��ַ���

sbit LCD_WR = P2^5;
sbit LCD_RS = P2^6;
sbit LCD_EN = P2^7;

#define LCD_DB P0

void delay_N_40us(int delayTime);
void LCDInit();  //��ʼ��LCD
char readLCDState();  //��ȡLCD��״̬
void writeLCDCommand(unsigned char cmd);  //��LCDдָ��
char readLCDData();  //��ȡLCD������
void writeLCDData(unsigned char dat);  //��LCDд����
void LCDDisplayChar(unsigned char x, unsigned char y, unsigned char dat);  //��LCDдһ���ַ�
void LCDDisplatString(unsigned char x, unsigned char y, char *str);  //��LCDд�ַ�������һ����ʾ������ַ�����ڵڶ��м�����ʾ
void LCDDisplatStringOverflow(unsigned char x, unsigned char y, char *str); //��LCDд�ַ�������һ����ʾ������ַ�����ڵ�һ�к��治��ʾ��λ��
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
	//while((readLCDState() & 0x80) != 0);  //���Һ����ʾ���Ƿ���æ��״̬ P0^7 = 1æ��P0^7 = 0����,����Ƿ���æ��״̬�ɹ��ʺܵ�

	writeLCDCommand(0x38);  //������������Ϊ8λ����ʾ2�У�ÿ�е��ַ���СΪ5*7
	writeLCDCommand(0x06);  //д�������ݹ�����ƣ���ʾ�����ƶ�	
	writeLCDCommand(0x0E);  //��ʾ���ܴ򿪣��й�꣬��겻��˸
	writeLCDCommand(0x14);  //�������һ����ACֵ��1
	writeLCDCommand(0x01);  //����
	
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
	writeLCDCommand(0x01);  //����
}

void delay_N_40us(int delayTime)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < delayTime;i++)
		for(j = 0;j < 5;j++ );
}


	