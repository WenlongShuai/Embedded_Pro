#include <reg51.h>

sbit d1 = P2^0;
sbit key1 = P3^1;

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

										
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
	int num = 0;
	
	P2 = 0xE3;  //�����λѡ��ѡ���һ�������

	while(1)
	{
		if(key1 == 0)  //���жϰ����Ƿ���
		{
			delayMs(5);  //������������ʱ5ms
			if(key1 == 0)  //�ٴμ�ⰴ���Ƿ���������
			{
				d1 = 0;
				P0 = table[num];  //����ܶ�ѡ
				num++;
				if(num == 10)
				{
					num = 0;
				}
				while(key1 == 0);  //�жϰ����Ƿ��ڰ���״̬
				delayMs(5);  //�ͷ���������ʱ5ms
				while(key1 == 0);  //�ٴ��ж��Ƿ��ڰ���״̬
			}
		}
		else
		{
			d1 = 1;
		}
	}
	return 0;
}

