#include <reg51.h>

//�ö�̬ɨ�跽ʽ�Ͷ�ʱ��1������ܵ�ǰ��λ��ʾ�����ȷ��1%��������λ��ʾ1%�룬һֱѭ����ȥ

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

void delayMs(unsigned int ms);
										
int count = 0;
int timeMs = 0;
int timeS = 0;
int main()
{
	EA = 1;  //�������ж�
	ET1 = 1;  //��ʱ��1���жϿ�������λ
	
	//���ö�ʱ��1�Ĺ�����ʽ�Լ�������ʱ��
	TMOD = 0x10;  //��ʽ1
	TR1 = 1;  //������ʱ��1  TCON = 0x40
	
	//���ó�ֵ
	TH1 = (65536 - 10000) / 256;
	TL1 = (65536 - 10000) % 256;
	
	while(1)
	{
		if(timeS == 10)
		{
			timeS = 0;
		}
		P2 = 0xEB;
		P0 = table[timeS] | 0x80;
		delayMs(1);
	}
	
	return 0;
}

void timer1() interrupt 3
{
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	count++;
	
	//10ms
	timeMs++;
	if(timeMs == 100)
	{
		timeMs = 0;
		timeS++;
	}

	P2 = 0xE3;
	P0 = table[timeMs % 10];
	delayMs(1);
	P2 = 0xE7;
	P0 = table[timeMs / 10];
	delayMs(2);  //�ȴ���ʾ�ȶ�
	P0 = 0x00;  //��Ӱ
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