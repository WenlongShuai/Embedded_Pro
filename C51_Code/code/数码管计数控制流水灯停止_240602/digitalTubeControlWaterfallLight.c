#include <reg51.h>
#include <intrins.h>

//���ö�̬ɨ��Ͷ�ʱ��1�����������ʾ����765432��ʼ��1/10����ٶ����µݼ�ֱ��765398��������ʾ������
//���ͬʱ���ö�ʱ��0��500ms�ٶȽ�����ˮ�ƴ��������ƶ�����������ϵ�������ֹͣʱ����ˮ��ֹͣȻ��ȫ��LED��ʼ��˸��
//3S����T0��ʱ������ˮ��ȫ���رգ��������ʾ��HELLO�������˱���ס

sbit bitSelectionA = P2^2;
sbit bitSelectionB = P2^3;
sbit bitSelectionC = P2^4;


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

char code hello[] = {0x76,0x7B,0x38,0x38,0x3F};
void selectionDigitalTube(unsigned char num);
void delayMs(unsigned int ms);

int timer0Count = 0;
int timer1Count = 0;										
							
long num = 765432;	
int i = 0;
long tmp = 0;
long count = 100000;
					
int main()
{

	//�����ж�
	EA = 1;   //���ж�
	ET0 = 1;  //��ʱ��0�ж��������λ
	ET1 = 1;  //��ʱ��1�ж��������λ
	
	//���ö�ʱ���Ĺ���ģʽ�Լ�������ʱ��
	TMOD = 0x11;  //��ʱ��0����ʱ��1�������ڷ�ʽһ
	TCON = 0x50;  //������ʱ��
	
	
	//���ó�ֵ
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;
	
	P2 = 0xFE;  //��ˮ�Ƴ�ʼ״̬����һ��LED
	
	while(1)
	{
		if(timer0Count < 60)   //�ж϶�ʱ��T0�Ƿ����3S�Ķ�ʱ
		{
			if(num == 765398)  //�ж�num�Ƿ�ݼ���765398������ݼ�����֮�󣬹رն�ʱ��1������LED�ƿ�ʼ��˸
			{
				TR1 = 0;
				P2 = 0x00;
				delayMs(100);
				P2 = 0xFF;
			}
			tmp = num;
			//��ʾ�ݼ�����
			for(i = 1;i<7;i++)
			{
				selectionDigitalTube(i);
				P0 = table[tmp / count];
				tmp %= count;
				count /= 10;
				P0 = 0x00;
			}
			count = 100000;
		}
		else    //��ʱ��0���3S��ʱ֮�󣬹ر�ȫ��LED�ƣ�����ʾ��HELLO��
		{
			P2 = 0xFF;
			for(i = 1;i<6;i++)
			{
				selectionDigitalTube(i);
				P0 = hello[i-1];
				delayMs(1);
				P0 = 0x00;
			}
		}
		
	}
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;
	timer0Count++;

	//�ж�num�Ƿ�ݼ���765398������ݼ�����֮��T0�Ϳ�ʼ3S�Ķ�ʱ�����ر���ˮ��
	if(timer0Count == 10 && num != 765398)
	{
		timer0Count = 0;
		P2 = _crol_(P2, 1);
	}
}

void timer1() interrupt 3
{
	TH1 = (65536 - 10000) / 256;  //10ms
	TL1 = (65536 - 10000) % 256;
	timer1Count++;
	
	//д��whileѭ���У����ܻ�����⣬����ԭ����ǿ���timer1Count=10��ʱ�򣬳���û��if�����������
	//�ȵ�if��������ʱ��timer1Count�Ѿ�������10�ˣ�����timer1Count��������
	if(timer1Count == 10)
	{
		timer1Count = 0;
		num--;
	}
}

void selectionDigitalTube(unsigned char num)
{
	switch(num)
	{
		case 1:
			//P2 = 0xFF;
			bitSelectionA = 1;
			bitSelectionB = 1;
			bitSelectionC = 1;
			break;
		case 2:
			//P2 = 0xFB;
			bitSelectionA = 0;
			bitSelectionB = 1;
			bitSelectionC = 1;
			break;
		case 3:
			//P2 = 0xF7;
			bitSelectionA = 1;
			bitSelectionB = 0;
			bitSelectionC = 1;
			break;
		case 4:
			//P2 = 0xF3;
			bitSelectionA = 0;
			bitSelectionB = 0;
			bitSelectionC = 1;
			break;
		case 5:
			//P2 = 0xEF;
			bitSelectionA = 1;
			bitSelectionB = 1;
			bitSelectionC = 0;
			break;
		case 6:
			//P2 = 0xEB;
			bitSelectionA = 0;
			bitSelectionB = 1;
			bitSelectionC = 0;
			break;
		case 7:
			//P2 = 0xE7;
			bitSelectionA = 1;
			bitSelectionB = 0;
			bitSelectionC = 0;
			break;
		case 8:
			//P2 = 0xE3;
			bitSelectionA = 0;
			bitSelectionB = 0;
			bitSelectionC = 0;
			break;
	}
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