#include <reg51.h>

//8*8LED点阵每秒显示一行LED,循环显示

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
	
	//在SHCP上升沿的时候，将8位数据依次移动到移位寄存器中
	for(i = 0;i < 8;i++)
	{
		//通过PSW程序状态寄存器移位的功能取出最高位
		SHCP = 0;
		serialDataInput <<= 1;
		DS = CY;
		SHCP = 1;	
		
		//通过数据右移多少位之后再&1取出最高位
//		SHCP = 0;
//		DS = serialDataInput >> (7 - i) & 1;
//		SHCP = 1;	
	}
	
	//在STCP上升沿的时候，将移位寄存器中的数据移到存储寄存器中
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
