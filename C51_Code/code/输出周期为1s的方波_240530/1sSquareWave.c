#include <reg51.h>

//利用定时/计数器T0从P2.0输出周期为1s的方波，让发光二极管以1HZ闪烁。晶振频率为12MHZ。

sbit outSquareWave = P2^0; 
int count = 0;

int main()
{
	//开中断
	EA = 1;   //开启总中断
	ET0 = 1;  //中断允许位
	
	//设置定时/计数器的工作模式以及启动停止寄存器
	TMOD = 0x01;  //方式1
	TCON = 0x10;  //设置TR0=1，启动定时/计数器
	
	//设置定时时间或计数次数
	//周期为1S   500ms产生一次中断
	TH0 = (65536-50000) / 256;  //定时时间为50ms
	TL0 = (65536-50000) % 256;
	
	
	while(1)
	{
		if(count == 20)
		{
			count = 0;
		}
		
		if(count < 10)
		{
			outSquareWave = 1;
		}
		else
		{
			outSquareWave = 0;
		}
	}
	
	return 0;
}

void timerInterrupt() interrupt 1
{
	TH0 = (65536-50000) / 256;  //定时时间为50ms
	TL0 = (65536-50000) % 256;
	count++;
}
