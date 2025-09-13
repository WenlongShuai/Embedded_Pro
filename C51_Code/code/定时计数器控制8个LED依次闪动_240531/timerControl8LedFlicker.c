#include <reg51.h>
#include <intrins.h>

//利用定时/计数器T1产生定时时钟，由P2口控制8个发光二极管，
//使8个LED依次闪烁，闪烁的频率为10次每秒（8个LED依次亮一遍为一个周期），循环闪烁

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}

int count = 0;
sbit beep = P1^5;

int main()
{
	P2 = 0xfe;
	//开中断
	EA = 1;   //开启总中断
	ET1 = 1;  //开启中断控制
	
	//设置T1工作模式以及启动和停止
	TMOD = 0x10;  
	TCON = 0x40;
	
	//设置定时时间  定时50ms
	TH1 = (65536-50000) / 256;
	TL1 = (65536-50000) % 256;
	
	while(1)
	{
		if(count == 20)
		{
			count = 0;
			beep = 1;
		}
		else
		{
			beep = 0;
			P2 = _crol_(P2, 1);
			delayMs(12);
		}
	}
	
	return 0;
}


void timerInterrupt() interrupt 3
{
	TH1 = (65536-50000) / 256;
	TL1 = (65536-50000) % 256;
	count++;
}