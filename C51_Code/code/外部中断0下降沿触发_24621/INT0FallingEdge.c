#include <reg51.h>

//K3������P3^2������ÿ����һ�ΰ���֮��P3^2�ھͻ����һ���½��أ��ͻᴥ���ⲿ�ж�0

int main()
{
	P2 = 0x00;
	//�����ж�
	EA = 1;
	
	//���ж�����λ
	EX0 = 1;
	
	//�жϴ�����ʽ���͵�ƽ�������½��ش�����
	IT0 = 1; //�½��ش���
	
	while(1);
	
	return 0;
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

void externalInt0() interrupt 0  
{
	P2++;
	delayMs(500);
}

