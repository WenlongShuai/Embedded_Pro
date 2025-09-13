#include <reg51.h>
#include <intrins.h>

//���ö�ʱ/������T1������ʱʱ�ӣ���P2�ڿ���8����������ܣ�
//ʹ8��LED������˸����˸��Ƶ��Ϊ10��ÿ�루8��LED������һ��Ϊһ�����ڣ���ѭ����˸

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}

int count = 0;
sbit beep = P1^5;

int main()
{
	P2 = 0xfe;
	//���ж�
	EA = 1;   //�������ж�
	ET1 = 1;  //�����жϿ���
	
	//����T1����ģʽ�Լ�������ֹͣ
	TMOD = 0x10;  
	TCON = 0x40;
	
	//���ö�ʱʱ��  ��ʱ50ms
	TH1 = (65536-50000) / 256;
	TL1 = (65536-50000) % 256;
	
	while(1)
	{
		if(count == 20)
		{
			count = 0;
			beep = 1;
		}
		else
		{
			beep = 0;
			P2 = _crol_(P2, 1);
			delayMs(12);
		}
	}
	
	return 0;
}


void timerInterrupt() interrupt 3
{
	TH1 = (65536-50000) / 256;
	TL1 = (65536-50000) % 256;
	count++;
}