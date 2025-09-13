#include <reg51.h>

//由上位机发送1给单片机时，蜂鸣器以400ms频率发声
//发送2给单片机时，以200ms频率发声
//发送3给单片机时，以100ms频率发声
//发送4给单片机时，关闭蜂鸣器

sbit beep = P1^5;
int timer0Count = 0;   //定时器0计数
unsigned char receiveData = 0;   //接收串口通信数据

int main()
{
	//打开总中断
	EA = 1;
	
	//打开串口中断
	ES = 1;
	
	//设置串行口的工作方式以及使能接收功能   方式1
	SCON = 0x50;   //SM0 = 0 SM1 = 1 REN =1
	
	//设置波特率  9600
	PCON = 0x00;   //SMOD = 0
	TH1 = 0xFD;  //定时器1的初值，通过波特率求出来的
	TL1 = 0xFD;
	
	//打开定时器T1，工作方式为方式2（8位自动重装）
	TMOD = 0x20;  //设置定时器T1的工作方式
	TR1 = 1;  //启动定时器

	//打开定时器T0，工作方式为方式1（16位）
	TMOD = 0x21;  //设置定时器T0的工作方式
	ET0 = 1;  //定时器0的中断允许控制位
	TR0 = 1;  //启动定时器
	
	//设置定时器T0的初值
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	
	while(1)
	{
		switch(receiveData)
		{
			case '1':
				if(timer0Count >= 4)
					beep = 1;
				else
					beep = 0;	
				break;
			case '2':
				if(timer0Count >= 2)
					beep = 1;
				else
					beep = 0;
				break;
			case '3':
				if(timer0Count >= 1)
					beep = 1;
				else
					beep = 0;
				break;
			case '4':
				beep = 1;
				break;
		}
	}
	return 0;
}

//串口中断服务程序
void serialInterrupt() interrupt 4
{
	if(RI)
	{
		RI = 0;
		receiveData = SBUF;
		timer0Count = 0; //如果切换不同的模式，就需要把timer0Count清零
		SBUF = receiveData;
		if(TI)
		{
			TI = 0;
		}
	}
}

//定时器0的中断服务程序
void timer0Interrupt() interrupt 1
{
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	timer0Count++;
	
	switch(receiveData)
	{
		case '1':
			if(timer0Count == 8)
				timer0Count = 0;
			break;
		case '2':
			if(timer0Count == 4)
				timer0Count = 0;
			break;
		case '3':
			if(timer0Count == 2)
				timer0Count = 0;
			break;
		case '4':
			timer0Count = 0;
			break;
	}
}