#include <reg51.h>
#include <stdio.h>

//利用UART方式实现printf、scanf函数

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
	char receiveData = 0;
	//配置定时/计数器T1
	EA = 1;  //打开总中断
	ES = 1;  //打开串口中断允许位
	
	//配置串口UART	
	SCON = 0x50;     //UART的工作方式，方式1(10位异步收发器)
	PCON = 0x00;   //波特率倍增位为0
	
	TH1 = 0xFD;  //波特率9600
	TL1 = 0xFD;
	
	//ET1 = 1; //打开中断允许控制位
	TMOD = 0x20;   //设置定时/计数器的工作方式，8位自动重装

	TR1 = 1;  //定时/计数器开始工作
	
	while(1)
	{
		scanf("%c",&receiveData);
		printf("------>\n");
		printf("---%c---\n", receiveData);
		//delayMs(1000);
	}
	
	return 0;
}


void UART_SendData(unsigned char dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}

char UART_ReadData()
{
	char dat;
	while(!RI);
	RI = 0;
	dat = SBUF;
	
	return dat;
}

char putchar(char ch)
{
	UART_SendData(ch);
	return ch;
}

char _getkey(void)
{
	return UART_ReadData();
}
