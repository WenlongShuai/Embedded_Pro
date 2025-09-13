#include <reg51.h>
#include <stdio.h>

//用AD以1HZ的频率采集模拟信号，然后转换成数字量，再将以1200bps的波特率发送到计算机，在计算机上显示

sbit XPT2046_CS = P3^5;
sbit XPT2046_DCLK = P3^6;
sbit XPT2046_DIN = P3^4;
sbit XPT2046_DOUT = P3^7;

int xpt2046_read_adc_value(unsigned char cmd);
void delayMs(unsigned int ms);

int timer0Count = 0;
float dataADC = 0.0f;
char str[10] = {0};

int main()
{
	int len = 0;
	int i = 0;
	//开启总中断
	EA = 1;
	
	//打开定时器T0的中断允许控制位
	ET0 = 1;
	//设置定时器T0的工作方式  方式1
	TMOD = 0x01;
	//打开定时器T0
	TR0 = 1;
	//设置定时器T0的初值
	TH0 = (65536 - 10000) / 256;
	TL0 = (65536 - 10000) % 256;
	
	//设置串口工作方式和启动串口  方式1
	SCON = 0x50;  //SM0 = 0 SM1 = 1 REN = 1
	//设置定时器T1的工作模式  方式2
	TMOD = 0x21;
	//设置波特率
	PCON = 0x00;  //SMOD = 0
	TH1 = 0xE8;
	TL1 = 0xE8;
	
	//打开定时器T1
	TR1 = 1;
	
	while(1)
	{	
		if(timer0Count == 10)
		{
			timer0Count = 0;
			dataADC = 5.0 * xpt2046_read_adc_value(0x97) / 4096;  //将模拟信号转换成数字信号
			
			sprintf(str, "%.2f\0", dataADC);  //使用sprintf函数时，str必须是字符数组
			
			i = 0;
			while(str[i] != '\0')
			{
				SBUF = str[i];
				while(!TI);
				TI = 0;
				i++;
			}
		}
	}
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65536 - 10000) / 256;
	TL0 = (65536 - 10000) % 256;
	timer0Count++;
}

void xpt2046_init()
{
	XPT2046_CS = 0;
	XPT2046_DCLK = 0;
	XPT2046_DIN = 0;
	XPT2046_DOUT = 0;
}

void xpt2046_write_data(unsigned char dat)
{
	int i = 0;

	for(i = 7;i >= 0;i--)
	{
		XPT2046_DCLK = 0;
		XPT2046_DIN = (dat >> i) & 1;
		XPT2046_DCLK = 1;
	}
	XPT2046_DIN = 0;

}

int xpt2046_read_data()
{
	int i = 0;
	int dat = 0;
	XPT2046_DOUT = 0;
	for(i = 0;i < 12;i++)
	{
		dat <<= 1;
		XPT2046_DCLK = 0;
		dat |= XPT2046_DOUT;
		XPT2046_DCLK = 1;
	}
	XPT2046_DOUT = 1;
	
	return dat;
}

int xpt2046_read_adc_value(unsigned char cmd)
{
	int dat = 0;
	xpt2046_init();
	xpt2046_write_data(cmd);
	
	delayMs(5);
	
	XPT2046_DCLK = 0;
	XPT2046_DCLK = 1;
	
	dat = xpt2046_read_data();
	
	XPT2046_DCLK = 0;
	XPT2046_CS = 1;
	
	return dat;
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
