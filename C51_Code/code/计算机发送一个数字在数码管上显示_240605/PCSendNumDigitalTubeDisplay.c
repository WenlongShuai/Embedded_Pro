#include <reg51.h>

//以16进制发送一个0-65535之间的任意数字，当单片机收到后数码管动态显示出来，波特率自定

sbit LSBA = P2^2;
sbit LSBB = P2^3;
sbit LSBC = P2^4;


unsigned int receiveData = 0;
unsigned char dataL = 0;
unsigned char dataH = 0;

unsigned int tmp = 0;

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

void delayMs(unsigned int ms);
void bitSelection(int num);

int main()
{
	int i = 1;
	
	//开启总中断
	EA = 1;
	
	//开启串口中断
	ES = 1;
	
	//设置定时器T1的工作模式
	TMOD = 0x20;  //方式2
	
	//打开定时器T1
	TR1 = 1;
	
	//设置串口的工作方式和打开串口接收允许位
	SCON = 0x50;  //方式1  REN = 1
	
	//波特率设置9600
	TH1 = 0xFD;
	TL1 = 0xFD;
	
	P2 = 0xFF;
	P0 = 0x00;
	
	while(1)
	{
		tmp = receiveData;
		while(tmp)
		{
			bitSelection(i);
			P0 = table[tmp % 10];
			tmp /= 10;
			i++;
			delayMs(1);
			P0 = 0x00;
		}
		i=1;
	}
	return 0;
}

void serialInterrupt() interrupt 4
{
	receiveData = 0;
	if(RI)
	{
		RI = 0;
		dataL = SBUF;
	}
	delayMs(100);
	if(RI)
	{
		RI = 0;
		dataH = SBUF;
		receiveData = dataH;
		receiveData <<= 8;
	}
	receiveData |= dataL;
	
//	SBUF = receiveData | 0x00;
//	while(!TI);
//	TI = 0;
//	SBUF = (receiveData >> 8) | 0x00;
//	while(!TI);
//	TI = 0;
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

void bitSelection(int num)
{
	switch(num)
	{
		case 1:
			LSBA = 0;
			LSBB = 0;
			LSBC = 0;
			//P2 = 0x00;
			break;
		case 2:
			LSBA = 1;
			LSBB = 0;
			LSBC = 0;
			//P2 = 0x04;
			break;
		case 3:
			LSBA = 0;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x08;
			break;
		case 4:
			LSBA = 1;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x0c;
			break;
		case 5:
			LSBA = 0;
			LSBB = 0;
			LSBC = 1;
			//P2 = 0x10;
			break;
		case 6:
			LSBA = 0;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x14;
			break;
		case 7:
			LSBA = 0;
			LSBB = 1;
			LSBC = 1;
			//P2 = 0x18;
			break;
		case 8:
			LSBA = 1;
			LSBB = 1;
			LSBC = 1;
			//P2 = 0x1c;
			break;
	}
}