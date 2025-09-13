#include <reg51.h>
#include <intrins.h>

//LED������ʾͼƬ�����Σ�

sbit SHCP = P3^6;
sbit STCP = P3^5;
sbit DS = P3^4;

#define uchar unsigned char
#define uint  unsigned int
	
#define LED_DOT_MATRIX_PORT P0  //LED�������
	

uchar code gled_row[]={0x38,0x7C,0x7E,0x3F,0x3F,0x7E,0x7C,0x38};//LED������ʾ����0��������
uchar code gled_col[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};//LED������ʾ����0��������

void hc595WriteData(uchar serialDataInput);
void delayMs(unsigned int ms);

int main()
{
	uchar i = 0;
	
	while(1)
	{
		if(i == 8)
			i = 0;
		hc595WriteData(gled_row[i]);
		LED_DOT_MATRIX_PORT = gled_col[i];
		delayMs(1);  //��ʾ�ȶ�
		//��Ӱ
		hc595WriteData(0x00);
		LED_DOT_MATRIX_PORT = 0xff;

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