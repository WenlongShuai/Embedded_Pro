#include <reg51.h>
#include <stdio.h>

//��AD��1HZ��Ƶ�ʲɼ�ģ���źţ�Ȼ��ת�������������ٽ���1200bps�Ĳ����ʷ��͵���������ڼ��������ʾ

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
	//�������ж�
	EA = 1;
	
	//�򿪶�ʱ��T0���ж��������λ
	ET0 = 1;
	//���ö�ʱ��T0�Ĺ�����ʽ  ��ʽ1
	TMOD = 0x01;
	//�򿪶�ʱ��T0
	TR0 = 1;
	//���ö�ʱ��T0�ĳ�ֵ
	TH0 = (65536 - 10000) / 256;
	TL0 = (65536 - 10000) % 256;
	
	//���ô��ڹ�����ʽ����������  ��ʽ1
	SCON = 0x50;  //SM0 = 0 SM1 = 1 REN = 1
	//���ö�ʱ��T1�Ĺ���ģʽ  ��ʽ2
	TMOD = 0x21;
	//���ò�����
	PCON = 0x00;  //SMOD = 0
	TH1 = 0xE8;
	TL1 = 0xE8;
	
	//�򿪶�ʱ��T1
	TR1 = 1;
	
	while(1)
	{	
		if(timer0Count == 10)
		{
			timer0Count = 0;
			dataADC = 5.0 * xpt2046_read_adc_value(0x97) / 4096;  //��ģ���ź�ת���������ź�
			
			sprintf(str, "%.2f\0", dataADC);  //ʹ��sprintf����ʱ��str�������ַ�����
			
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
