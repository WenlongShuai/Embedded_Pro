#include <reg51.h>

//���ö�ʱ/������T0��P2.0�������Ϊ1s�ķ������÷����������1HZ��˸������Ƶ��Ϊ12MHZ��

sbit outSquareWave = P2^0; 
int count = 0;

int main()
{
	//���ж�
	EA = 1;   //�������ж�
	ET0 = 1;  //�ж�����λ
	
	//���ö�ʱ/�������Ĺ���ģʽ�Լ�����ֹͣ�Ĵ���
	TMOD = 0x01;  //��ʽ1
	TCON = 0x10;  //����TR0=1��������ʱ/������
	
	//���ö�ʱʱ����������
	//����Ϊ1S   500ms����һ���ж�
	TH0 = (65536-50000) / 256;  //��ʱʱ��Ϊ50ms
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
	TH0 = (65536-50000) / 256;  //��ʱʱ��Ϊ50ms
	TL0 = (65536-50000) % 256;
	count++;
}
