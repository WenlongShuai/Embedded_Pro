#include <reg51.h>
#include <intrins.h>

//利用动态扫描和定时器1在数码管上显示出从765432开始以1/10秒的速度往下递减直至765398并保持显示此数，
//与此同时利用定时器0以500ms速度进行流水灯从上往下移动，当数码管上的数减到停止时，流水灯停止然后全部LED开始闪烁，
//3S后（用T0定时），流水灯全部关闭，数码管显示“HELLO”，到此保持住

sbit bitSelectionA = P2^2;
sbit bitSelectionB = P2^3;
sbit bitSelectionC = P2^4;


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

char code hello[] = {0x76,0x7B,0x38,0x38,0x3F};
void selectionDigitalTube(unsigned char num);
void delayMs(unsigned int ms);

int timer0Count = 0;
int timer1Count = 0;										
							
long num = 765432;	
int i = 0;
long tmp = 0;
long count = 100000;
					
int main()
{

	//开启中断
	EA = 1;   //总中断
	ET0 = 1;  //定时器0中断允许控制位
	ET1 = 1;  //定时器1中断允许控制位
	
	//设置定时器的工作模式以及启动定时器
	TMOD = 0x11;  //定时器0、定时器1都工作在方式一
	TCON = 0x50;  //启动定时器
	
	
	//设置初值
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;
	
	P2 = 0xFE;  //流水灯初始状态，亮一颗LED
	
	while(1)
	{
		if(timer0Count < 60)   //判断定时器T0是否完成3S的定时
		{
			if(num == 765398)  //判断num是否递减到765398，如果递减到了之后，关闭定时器1，所有LED灯开始闪烁
			{
				TR1 = 0;
				P2 = 0x00;
				delayMs(100);
				P2 = 0xFF;
			}
			tmp = num;
			//显示递减的数
			for(i = 1;i<7;i++)
			{
				selectionDigitalTube(i);
				P0 = table[tmp / count];
				tmp %= count;
				count /= 10;
				P0 = 0x00;
			}
			count = 100000;
		}
		else    //定时器0完成3S定时之后，关闭全部LED灯，并显示“HELLO”
		{
			P2 = 0xFF;
			for(i = 1;i<6;i++)
			{
				selectionDigitalTube(i);
				P0 = hello[i-1];
				delayMs(1);
				P0 = 0x00;
			}
		}
		
	}
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;
	timer0Count++;

	//判断num是否递减到765398，如果递减到了之后，T0就开始3S的定时，并关闭流水灯
	if(timer0Count == 10 && num != 765398)
	{
		timer0Count = 0;
		P2 = _crol_(P2, 1);
	}
}

void timer1() interrupt 3
{
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	timer1Count++;
	
	//写在while循环中，可能会出问题，问题原因就是可能timer1Count=10的时候，程序还没到if语句那里来，
	//等到if语句那里的时候，timer1Count已经不等于10了，导致timer1Count不能清零
	if(timer1Count == 10)
	{
		timer1Count = 0;
		num--;
	}
}

void selectionDigitalTube(unsigned char num)
{
	switch(num)
	{
		case 1:
			//P2 = 0xFF;
			bitSelectionA = 1;
			bitSelectionB = 1;
			bitSelectionC = 1;
			break;
		case 2:
			//P2 = 0xFB;
			bitSelectionA = 0;
			bitSelectionB = 1;
			bitSelectionC = 1;
			break;
		case 3:
			//P2 = 0xF7;
			bitSelectionA = 1;
			bitSelectionB = 0;
			bitSelectionC = 1;
			break;
		case 4:
			//P2 = 0xF3;
			bitSelectionA = 0;
			bitSelectionB = 0;
			bitSelectionC = 1;
			break;
		case 5:
			//P2 = 0xEF;
			bitSelectionA = 1;
			bitSelectionB = 1;
			bitSelectionC = 0;
			break;
		case 6:
			//P2 = 0xEB;
			bitSelectionA = 0;
			bitSelectionB = 1;
			bitSelectionC = 0;
			break;
		case 7:
			//P2 = 0xE7;
			bitSelectionA = 1;
			bitSelectionB = 0;
			bitSelectionC = 0;
			break;
		case 8:
			//P2 = 0xE3;
			bitSelectionA = 0;
			bitSelectionB = 0;
			bitSelectionC = 0;
			break;
	}
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