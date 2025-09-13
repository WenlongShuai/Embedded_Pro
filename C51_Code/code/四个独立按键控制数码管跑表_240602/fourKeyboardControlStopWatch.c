#include <reg51.h>

//�����ǰ��λ��ʾһ���ܱ���000-999֮����1%����ٶ����У�
//4�����������ֱ������ͣ��ֹͣ����ʼ������
//��һ��������������ʱ�ܱ�ֹͣ���ɿ��ֺ��ܱ�������С�
//�ڶ�������������ʱ��ʱֹͣ��
//����������������ʱ��ʱ��ʼ��
//���ĸ�����������ʱ����ֵ���㡣

sbit K1 = P3^1;
sbit K2 = P3^0;
sbit K3 = P3^2;
sbit K4 = P3^3;

void selectionDigitalTube(unsigned char);
void delayMs(unsigned int);

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

int num = 0;
int timer0Count = 0;
int tmp = 0;

int main()
{
	EA = 1;  //�����ж�
	ET0 = 1;  //��ʱ��0���ж��������λ
	
	TMOD = 0x01;   //����ģʽΪ��ʽ1
	TCON = 0x10;   //������ʱ��T0
	
	//���ó�ֵ
	TH0 = (65535-1000) / 256;  //1ms
	TL0 = (65535-1000) % 256;
	
	P3 = 0xFF;  //P1-P3��׼˫��ڡ�P3���������ʱ����Ҫ������io�ڣ����������Ҫ
	
	while(1)
	{
			tmp = P3;
			if(num >= 1000)
			{
				num = 0;
			}
			switch(tmp)
			{
				case 0xFD:  //��һ����������
					if(!K1)  
					{
						delayMs(5);  //��������
						while(K1);
						TR0 = 0;
						while(!K1)
						{
							selectionDigitalTube(1);
							if(num > 99)
								P0 = table[num / 100];
							else
								P0 = table[0];
							delayMs(1);
							P0 = 0x00;   //��Ӱ
						
							selectionDigitalTube(2);
							if(num > 9)
								P0 = table[num % 100 / 10];
							else
								P0 = table[0];
							delayMs(1);
							P0 = 0x00;   //��Ӱ
						
							selectionDigitalTube(3);
							P0 = table[num % 100 % 10];
							delayMs(1);
							P0 = 0x00;   //��Ӱ
						}
						delayMs(5);  //�ͷ�����
						while(!K1);
						TR0 = 1;
					}
					break; 
				case 0xFE:  //�ڶ�����������
					if(!K2)
					{
						delayMs(5);
						while(K2);
						TR0 = 0;
						delayMs(5);
						while(!K2);
					}
					break;
				case 0xFB:  //��������������
					if(!K3)
					{
						delayMs(5);
						while(K3);
						TR0 = 1;
						delayMs(5);
						while(!K3);
					}
					break;
				case 0xF7:   //���ĸ���������
					if(!K4)
					{
						delayMs(5);
						while(K4);
						num = 0;
						delayMs(5);
						while(!K4);
					}
					break;
			}

			selectionDigitalTube(1);
			if(num > 99)
				P0 = table[num / 100];
			else
				P0 = table[0];
			delayMs(1);
			P0 = 0x00;   //��Ӱ
		
			selectionDigitalTube(2);
			if(num > 9)
				P0 = table[num % 100 / 10];
			else
				P0 = table[0];
			delayMs(1);
			P0 = 0x00;   //��Ӱ
		
			selectionDigitalTube(3);
			P0 = table[num % 100 % 10];
			delayMs(1);
			P0 = 0x00;   //��Ӱ
	}
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65535-1000) / 256;  //1ms
	TL0 = (65535-1000) % 256;
	timer0Count++;
	
	//ΪʲôҪд������
	//��Ϊ����timer0Count=10��ʱ�򣬳�����whileѭ���е�forѭ���У�
	//�´���ȥ�ж�timer0Count==10��ʱ�򣬾Ϳ��ܻ������ͻᵼ��timer0Count���ܱ����㡣
	//���Է����жϷ������У�ÿ�ж�һ�ξ����ж�һ�Σ������ͻ��������Ǹ�����
	if(timer0Count == 10)
	{
		timer0Count = 0;
		num++;
	}
}

void selectionDigitalTube(unsigned char num)
{
	switch(num)
	{
		case 1:
			P2 = 0xFF;
			break;
		case 2:
			P2 = 0xFB;
			break;
		case 3:
			P2 = 0xF7;
			break;
		case 4:
			P2 = 0xF3;
			break;
		case 5:
			P2 = 0xEF;
			break;
		case 6:
			P2 = 0xEB;
			break;
		case 7:
			P2 = 0xE7;
			break;
		case 8:
			P2 = 0xE3;
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
