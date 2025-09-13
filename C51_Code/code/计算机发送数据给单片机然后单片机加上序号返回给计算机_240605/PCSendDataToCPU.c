#include <reg51.h>
#include <stdio.h>

//以2400bps从计算机发送任一字节数据，当单片机收到该数据后，
//在此数据前加上序号，然后连同此数据一起发送至计算机，当序号超过255时归零。

int receiveData = 0;
int count = 0;
int flag = 0;

int main()
{
	//开启总中断
	EA = 1;
	
	//开启串口中断
	ES = 1;
	
	//打开定时器T1
	TR1 = 1;
	
	//设置定时器T1的工作方式
	TMOD = 0x20;
	
	//设置串口的工作方式
	SCON = 0x50;   //方式二  REN = 1
	
	//设置SMOD = 0 采样的比特率不加倍
	PCON = 0x00;
	
	//设置波特率 2400
	TH1 = 0xF4;
	TL1 = 0xF4;
	
	
	while(1)
	{
		if(flag)
		{
			flag = 0;
			SBUF = count;
			while(!TI);
			TI = 0;
			count++;
			SBUF = receiveData;
			while(!TI);
			TI = 0;
		}
	}
	return 0;
}

//TI、RI为高电平的时候触发此中断服务函数
void serialInterrupt() interrupt 4
{
	if(RI)
	{
		RI = 0;
		if(count == 256)
		{
			count = 0;
		}
		receiveData = SBUF;
		
		flag = 1;
	}
}