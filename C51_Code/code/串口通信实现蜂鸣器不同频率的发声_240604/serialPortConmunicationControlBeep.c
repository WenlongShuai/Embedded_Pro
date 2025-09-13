#include <reg51.h>

//����λ������1����Ƭ��ʱ����������400msƵ�ʷ���
//����2����Ƭ��ʱ����200msƵ�ʷ���
//����3����Ƭ��ʱ����100msƵ�ʷ���
//����4����Ƭ��ʱ���رշ�����

sbit beep = P1^5;
int timer0Count = 0;   //��ʱ��0����
unsigned char receiveData = 0;   //���մ���ͨ������

int main()
{
	//�����ж�
	EA = 1;
	
	//�򿪴����ж�
	ES = 1;
	
	//���ô��пڵĹ�����ʽ�Լ�ʹ�ܽ��չ���   ��ʽ1
	SCON = 0x50;   //SM0 = 0 SM1 = 1 REN =1
	
	//���ò�����  9600
	PCON = 0x00;   //SMOD = 0
	TH1 = 0xFD;  //��ʱ��1�ĳ�ֵ��ͨ���������������
	TL1 = 0xFD;
	
	//�򿪶�ʱ��T1��������ʽΪ��ʽ2��8λ�Զ���װ��
	TMOD = 0x20;  //���ö�ʱ��T1�Ĺ�����ʽ
	TR1 = 1;  //������ʱ��

	//�򿪶�ʱ��T0��������ʽΪ��ʽ1��16λ��
	TMOD = 0x21;  //���ö�ʱ��T0�Ĺ�����ʽ
	ET0 = 1;  //��ʱ��0���ж��������λ
	TR0 = 1;  //������ʱ��
	
	//���ö�ʱ��T0�ĳ�ֵ
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

//�����жϷ������
void serialInterrupt() interrupt 4
{
	if(RI)
	{
		RI = 0;
		receiveData = SBUF;
		timer0Count = 0; //����л���ͬ��ģʽ������Ҫ��timer0Count����
		SBUF = receiveData;
		if(TI)
		{
			TI = 0;
		}
	}
}

//��ʱ��0���жϷ������
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