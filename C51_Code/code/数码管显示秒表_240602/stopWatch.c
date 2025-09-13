#include <reg51.h>

//用动态扫描方式和定时器1在数码管的前三位显示秒表，精确到1%，即后两位显示1%秒，一直循环下去

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

void delayMs(unsigned int ms);
										
int count = 0;
int timeMs = 0;
int timeS = 0;
int main()
{
	EA = 1;  //开启总中断
	ET1 = 1;  //定时器1的中断控制允许位
	
	//设置定时器1的工作方式以及启动定时器
	TMOD = 0x10;  //方式1
	TR1 = 1;  //启动定时器1  TCON = 0x40
	
	//设置初值
	TH1 = (65536 - 10000) / 256;
	TL1 = (65536 - 10000) % 256;
	
	while(1)
	{
		if(timeS == 10)
		{
			timeS = 0;
		}
		P2 = 0xEB;
		P0 = table[timeS] | 0x80;
		delayMs(1);
	}
	
	return 0;
}

void timer1() interrupt 3
{
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	count++;
	
	//10ms
	timeMs++;
	if(timeMs == 100)
	{
		timeMs = 0;
		timeS++;
	}

	P2 = 0xE3;
	P0 = table[timeMs % 10];
	delayMs(1);
	P2 = 0xE7;
	P0 = table[timeMs / 10];
	delayMs(2);  //等待显示稳定
	P0 = 0x00;  //消影
}


void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}