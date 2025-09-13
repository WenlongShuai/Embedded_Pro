#include <reg51.h>
//利用动态扫描方法在六位数码管上显示稳定的87654321
//使用定时器做数码管的延时

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

int count = 0;
int num = 1;							
										
void selectionDigitalTube(unsigned char num);

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}
										
int main()
{
	//打开中断
	EA = 1;  //打开总中断
	ET1 = 1;  //中断控制允许位
	
	//设置定时器1的工作方式以及启动停止
	TMOD = 0x10;  //工作方式为方式1
	TCON = 0x40;  //启动定时器  TR1 = 1
	
	//设置初值
	TH1 = (65536 - 500) / 256;  //0.5ms
	TL1 = (65536 - 500) % 256;
	
	while(1)
	{
		
		//定时器的方法
		if(count == 2)  //1ms
		{
			count = 0;
			if(num == 9)
			{
				num = 1;
			}
			selectionDigitalTube(num);
			P0 = table[num];
			num++;
		}
		
		/*
		//延时的方法
		if(num == 9)
		{
			num = 1;
		}
		selectionDigitalTube(num);
		P0 = table[num];
		num++;
		
		delayMs(1); 
		*/
			
		
	}
	
	
	return 0;
}

void timer1() interrupt 3
{
	TH1 = (65536 - 500) / 256;
	TL1 = (65536 - 500) % 256;
	count++;
}

void selectionDigitalTube(unsigned char num)
{
	switch(num)
	{
		case 1:
			P2 = 0xFF;
			break;
		case 2:
			P2 = 0xFB;
			break;
		case 3:
			P2 = 0xF7;
			break;
		case 4:
			P2 = 0xF3;
			break;
		case 5:
			P2 = 0xEF;
			break;
		case 6:
			P2 = 0xEB;
			break;
		case 7:
			P2 = 0xE7;
			break;
		case 8:
			P2 = 0xE3;
			break;
	}
}