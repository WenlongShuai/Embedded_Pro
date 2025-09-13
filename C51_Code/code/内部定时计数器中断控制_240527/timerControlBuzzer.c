#include <reg51.h>
//ʹ�ö�ʱ/���������Ʒ�����
//ʹ�÷�ʽ1
//��ʱ1s��ʱ�䵽����������

sbit buzzer = P1^5;
int count = 0;

int main()
{
	EA = 1;  //�����ж�
	ET0 = 1;  //�ж��������,�ж�����λ
	
	//��ʱ/�������Ĺ�����ʽ������ֹͣ����
	//TMOD �� TCON �����Ĵ������п���
	TMOD = 0x01;  
	TCON = 0x10;
	
	//������ʽ  50ms
	TH0 = (65535-50000) / 256;
	TL0 = (65535-50000) % 256;
	
	while(1)
	{ 
		if(count == 20)
		{
			buzzer = 0;
			count = 0;
		}
		else
		{
			buzzer = 1; 
		}
		
	}
	
	return 0;
}

void timerInterrupt() interrupt 1
{
	TH0 = (65535-50000) / 256;
	TL0 = (65535-50000) % 256;
	count++;
}