#include <reg51.h>

//数码管前三位显示一个跑表，从000-999之间以1%秒的速度运行，
//4个独立按键分别控制暂停、停止、开始、清零
//第一个按键，当按下时跑表停止，松开手后跑表继续运行。
//第二个按键，按下时计时停止。
//第三个按键，按下时计时开始。
//第四个按键，按下时计数值清零。

sbit K1 = P3^1;
sbit K2 = P3^0;
sbit K3 = P3^2;
sbit K4 = P3^3;

void selectionDigitalTube(unsigned char);
void delayMs(unsigned int);

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

int num = 0;
int timer0Count = 0;
int tmp = 0;

int main()
{
	EA = 1;  //打开总中断
	ET0 = 1;  //定时器0的中断允许控制位
	
	TMOD = 0x01;   //工作模式为方式1
	TCON = 0x10;   //启动定时器T0
	
	//设置初值
	TH0 = (65535-1000) / 256;  //1ms
	TL0 = (65535-1000) % 256;
	
	P3 = 0xFF;  //P1-P3是准双向口。P3口作输入的时候，需要先拉高io口，作输出不需要
	
	while(1)
	{
			tmp = P3;
			if(num >= 1000)
			{
				num = 0;
			}
			switch(tmp)
			{
				case 0xFD:  //第一个按键按下
					if(!K1)  
					{
						delayMs(5);  //按下消抖
						while(K1);
						TR0 = 0;
						while(!K1)
						{
							selectionDigitalTube(1);
							if(num > 99)
								P0 = table[num / 100];
							else
								P0 = table[0];
							delayMs(1);
							P0 = 0x00;   //消影
						
							selectionDigitalTube(2);
							if(num > 9)
								P0 = table[num % 100 / 10];
							else
								P0 = table[0];
							delayMs(1);
							P0 = 0x00;   //消影
						
							selectionDigitalTube(3);
							P0 = table[num % 100 % 10];
							delayMs(1);
							P0 = 0x00;   //消影
						}
						delayMs(5);  //释放消抖
						while(!K1);
						TR0 = 1;
					}
					break; 
				case 0xFE:  //第二个按键按下
					if(!K2)
					{
						delayMs(5);
						while(K2);
						TR0 = 0;
						delayMs(5);
						while(!K2);
					}
					break;
				case 0xFB:  //第三个按键按下
					if(!K3)
					{
						delayMs(5);
						while(K3);
						TR0 = 1;
						delayMs(5);
						while(!K3);
					}
					break;
				case 0xF7:   //第四个按键按下
					if(!K4)
					{
						delayMs(5);
						while(K4);
						num = 0;
						delayMs(5);
						while(!K4);
					}
					break;
			}

			selectionDigitalTube(1);
			if(num > 99)
				P0 = table[num / 100];
			else
				P0 = table[0];
			delayMs(1);
			P0 = 0x00;   //消影
		
			selectionDigitalTube(2);
			if(num > 9)
				P0 = table[num % 100 / 10];
			else
				P0 = table[0];
			delayMs(1);
			P0 = 0x00;   //消影
		
			selectionDigitalTube(3);
			P0 = table[num % 100 % 10];
			delayMs(1);
			P0 = 0x00;   //消影
	}
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65535-1000) / 256;  //1ms
	TL0 = (65535-1000) % 256;
	timer0Count++;
	
	//为什么要写在这里
	//因为可能timer0Count=10的时候，程序在while循环中的for循环中，
	//下次再去判断timer0Count==10的时候，就可能会错过，就会导致timer0Count不能被置零。
	//所以放在中断服务函数中，每中断一次就来判断一次，这样就会解决上面那个问题
	if(timer0Count == 10)
	{
		timer0Count = 0;
		num++;
	}
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

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}
