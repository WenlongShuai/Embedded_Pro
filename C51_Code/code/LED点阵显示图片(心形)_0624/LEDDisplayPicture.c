#include <reg51.h>
#include <intrins.h>

//LED点阵显示图片（心形）

sbit SHCP = P3^6;
sbit STCP = P3^5;
sbit DS = P3^4;

#define uchar unsigned char
#define uint  unsigned int
	
#define LED_DOT_MATRIX_PORT P0  //LED点阵的列
	

uchar code gled_row[]={0x38,0x7C,0x7E,0x3F,0x3F,0x7E,0x7C,0x38};//LED点阵显示数字0的行数据
uchar code gled_col[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};//LED点阵显示数字0的列数据

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
		delayMs(1);  //显示稳定
		//消影
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