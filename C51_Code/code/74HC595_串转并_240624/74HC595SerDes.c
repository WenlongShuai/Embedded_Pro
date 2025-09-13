#include <reg51.h>

//8*8LED����ÿ����ʾһ��LED,ѭ����ʾ

sbit SHCP = P3^6;
sbit STCP = P3^5;
sbit DS = P3^4;

#define LEDDotMatrix P0

#define uchar unsigned char
#define uint  unsigned int
	
uchar code hc595_buf[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void hc595WriteData(uchar serialDataInput);
void delayMs(unsigned int ms);

int main()
{
	uchar i = 0;
	while(1)
	{
		if(i == 8)
			i = 0;
		hc595WriteData(hc595_buf[i]);
		LEDDotMatrix = 0x00;
		delayMs(1000);
		i++;
	}
	return 0;
}

void hc595WriteData(uchar serialDataInput)
{
	uchar i = 0;
	SHCP = 0;
	STCP = 0;
	
	//��SHCP�����ص�ʱ�򣬽�8λ���������ƶ�����λ�Ĵ�����
	for(i = 0;i < 8;i++)
	{
		//ͨ��PSW����״̬�Ĵ�����λ�Ĺ���ȡ�����λ
		SHCP = 0;
		serialDataInput <<= 1;
		DS = CY;
		SHCP = 1;	
		
		//ͨ���������ƶ���λ֮����&1ȡ�����λ
//		SHCP = 0;
//		DS = serialDataInput >> (7 - i) & 1;
//		SHCP = 1;	
	}
	
	//��STCP�����ص�ʱ�򣬽���λ�Ĵ����е������Ƶ��洢�Ĵ�����
	STCP = 1;	
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
