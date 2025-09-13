#include <reg51.h>
//使用定时/计数器控制蜂鸣器
//使用方式1
//定时1s，时间到蜂鸣器就响

sbit buzzer = P1^5;
int count = 0;

int main()
{
	EA = 1;  //打开总中断
	ET0 = 1;  //中断允许控制,中断允许位
	
	//定时/计数器的工作方式和启动停止设置
	//TMOD 和 TCON 两个寄存器进行控制
	TMOD = 0x01;  
	TCON = 0x10;
	
	//触发方式  50ms
	TH0 = (65535-50000) / 256;
	TL0 = (65535-50000) % 256;
	
	while(1)
	{ 
		if(count == 20)
		{
			buzzer = 0;
			count = 0;
		}
		else
		{
			buzzer = 1; 
		}
		
	}
	
	return 0;
}

void timerInterrupt() interrupt 1
{
	TH0 = (65535-50000) / 256;
	TL0 = (65535-50000) % 256;
	count++;
}