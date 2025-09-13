#include <reg51.h>

//ͨ��DAת��������PWM��������Ϊ1ms��ռ�ձ�Ϊ0%-80%���α仯

sbit DAC1 = P2^1;

int count = 0;  //���ƽ����жϵĴ���
int dutyCycle = 0;   //ռ�ձ�
int dir = 0;   //���Ʒ���dir=0ʱ��ռ�ձȴ�0-80��dir=1ʱռ�ձȴ�80-0

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
	//���ж�
	EA = 1;  //�����ж�
	ET0 = 1;  //��ʱ/������0 �ж�����λ
	
	//���� ��ʱ/������0 ������ʽ������ֹͣ
	TMOD = 0x01;  //������ʽ1
	TCON = 0x10;  //���� ��ʱ/������0
	
	//���ö�ʱʱ�䣬��ʼ����ʱʱ��Ϊ0
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
	TH0 = (65536-10) / 256; //��ʱʱ��Ϊ0.01ms
	TL0 = (65536-10) % 256;
	count++;
	
	if(count == 100)  //һ�����ڽ���,һ������Ϊ1ms
	{
		count = 0;
		delayMs(30);
	}
	
	if(count <= dutyCycle)  //һ�����ڸߵ�ƽ���ģ���ѹ��ʱ��
	{
		DAC1 = 1;
	}
	else
	{
		DAC1 = 0;	
	}
}