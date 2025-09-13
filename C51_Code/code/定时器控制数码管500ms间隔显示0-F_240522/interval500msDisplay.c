#include <reg51.h>

//�ö�ʱ����500ms�ļ����6λ�������������ʾ0-F���ظ�


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

int count = 0;
										
int main()
{
	int num = 0;
	//���ж�
	EA = 1;  //���ж�
	ET1 = 1;  //��ʱ/������1���жϿ���λ
	
	//����ģʽ�Լ�����ֹͣ����
	TMOD = 0x10;  //������ʽΪ��ʽ1
	TCON = 0x40;   //������ʱ��
	
	//���ö�ʱ���Ķ�ʱʱ��
	TH1 = (65536-50000) / 256;  
	TL1 = (65536-50000) % 256;
	
	P2 = 0x1C;   //λѡ
	P0 = 0x00;   //��ѡ
	
	while(1)
	{
		if(count == 10)  //��ʱ500msʱ�䵽
		{
			if(num == 16)
			{
				num = 0;
			}
		
			P0 = table[num];
			num++;
			P2 -= 4;
			
			if(P2+4 == 0x00)
			{
				P2 = 0x1C;
			}
			count = 0;
		}
	}
	return 0;
}


//��ʱ/������1
void timerInterrupt1() interrupt 3
{
	TH1 = (65536-50000) / 256;  
	TL1 = (65536-50000) % 256;
	count++;
}