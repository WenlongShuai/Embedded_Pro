#include <reg51.h>

//通过DA转换器产生PWM波，周期为1ms，占空比为0%-80%依次变化

sbit DAC1 = P2^1;

int count = 0;  //控制进入中断的次数
int dutyCycle = 0;   //占空比
int dir = 0;   //控制方向，dir=0时，占空比从0-80，dir=1时占空比从80-0

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
	ET0 = 1;  //定时/计数器0 中断允许位
	
	//设置 定时/计数器0 工作方式、启动停止
	TMOD = 0x01;  //工作方式1
	TCON = 0x10;  //启动 定时/计数器0
	
	//设置定时时间，初始化定时时间为0
	TH0 = (65536-0) / 256;
	TL0 = (65536-0) % 256;
	
	while(1)
	{
		if(dir == 0)  
		{
			dutyCycle++;
			if(dutyCycle == 80)
			{
				dir = 1;
			}
		}
		else
		{
			dutyCycle--;
			if(dutyCycle == 0)
			{
				dir = 0;
			}
		}
	}
	return 0;
}

void timerInterrupt0() interrupt 1
{
	TH0 = (65536-10) / 256; //定时时间为0.01ms
	TL0 = (65536-10) % 256;
	count++;
	
	if(count == 100)  //一个周期结束,一个周期为1ms
	{
		count = 0;
		delayMs(30);
	}
	
	if(count <= dutyCycle)  //一个周期高电平输出模拟电压的时间
	{
		DAC1 = 1;
	}
	else
	{
		DAC1 = 0;	
	}
}