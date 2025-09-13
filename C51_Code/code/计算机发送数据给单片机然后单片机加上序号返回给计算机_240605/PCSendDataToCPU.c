#include <reg51.h>
#include <stdio.h>

//��2400bps�Ӽ����������һ�ֽ����ݣ�����Ƭ���յ������ݺ�
//�ڴ�����ǰ������ţ�Ȼ����ͬ������һ�����������������ų���255ʱ���㡣

int receiveData = 0;
int count = 0;
int flag = 0;

int main()
{
	//�������ж�
	EA = 1;
	
	//���������ж�
	ES = 1;
	
	//�򿪶�ʱ��T1
	TR1 = 1;
	
	//���ö�ʱ��T1�Ĺ�����ʽ
	TMOD = 0x20;
	
	//���ô��ڵĹ�����ʽ
	SCON = 0x50;   //��ʽ��  REN = 1
	
	//����SMOD = 0 �����ı����ʲ��ӱ�
	PCON = 0x00;
	
	//���ò����� 2400
	TH1 = 0xF4;
	TL1 = 0xF4;
	
	
	while(1)
	{
		if(flag)
		{
			flag = 0;
			SBUF = count;
			while(!TI);
			TI = 0;
			count++;
			SBUF = receiveData;
			while(!TI);
			TI = 0;
		}
	}
	return 0;
}

//TI��RIΪ�ߵ�ƽ��ʱ�򴥷����жϷ�����
void serialInterrupt() interrupt 4
{
	if(RI)
	{
		RI = 0;
		if(count == 256)
		{
			count = 0;
		}
		receiveData = SBUF;
		
		flag = 1;
	}
}