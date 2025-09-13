#include <reg51.h>

//用定时器以500ms的间隔在6位数码管上依次显示0-F，重复


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

int count = 0;
										
int main()
{
	int num = 0;
	//打开中断
	EA = 1;  //总中断
	ET1 = 1;  //定时/计数器1的中断控制位
	
	//设置模式以及启动停止设置
	TMOD = 0x10;  //工作方式为方式1
	TCON = 0x40;   //启动定时器
	
	//设置定时器的定时时间
	TH1 = (65536-50000) / 256;  
	TL1 = (65536-50000) % 256;
	
	P2 = 0x1C;   //位选
	P0 = 0x00;   //段选
	
	while(1)
	{
		if(count == 10)  //定时500ms时间到
		{
			if(num == 16)
			{
				num = 0;
			}
		
			P0 = table[num];
			num++;
			P2 -= 4;
			
			if(P2+4 == 0x00)
			{
				P2 = 0x1C;
			}
			count = 0;
		}
	}
	return 0;
}


//定时/计数器1
void timerInterrupt1() interrupt 3
{
	TH1 = (65536-50000) / 256;  
	TL1 = (65536-50000) % 256;
	count++;
}