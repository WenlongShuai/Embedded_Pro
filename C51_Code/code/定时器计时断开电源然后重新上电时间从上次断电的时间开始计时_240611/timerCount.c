#include <reg51.h>
#include "I2C.h"

sbit bitSelectionA = P2^2;
sbit bitSelectionB = P2^3;
sbit bitSelectionC = P2^4;


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

uint timer0Count = 0;
uint count = 0;
										
void selectionDigitalTube(unsigned char num);

int main()
{
	uchar i = 0;
	uint tmp = 0;
	uchar eepromAddr = 1;
	
	//打开总中断
	EA = 1;
	
	//打开定时器T0 的中断控制允许位
	ET0 = 1;
	
	//设置定时器T0的工作方式
	TMOD = 0x01;   //方式1（16位）
	
	//启动定时器T0
	TR0 = 1;
	
	//设置初值
	TH0 = (65536 - 10000) / 256;  //10ms
	TL0 = (65536 - 10000) % 256;
	
	while(1)
	{
		tmp = eepromReadRandomAddr(0xa1, i+1);
		count += tmp;
		
		if(tmp != 255)
			break;
		i++;
	}
	
	i = 0;
	tmp = 0;
	
	
	while(1)
	{
		if(timer0Count == 100)
		{
			timer0Count = 0;
			count++;
			eepromAddr = count / 256 + 1;
			eepromWriteByte(0xa0, eepromAddr, count%256);
		}
		
		tmp = count;
		
		while(tmp)
		{
			selectionDigitalTube(8-i);
			P0 = table[tmp % 10];
			delayMs(1);
			P0 = 0x00;
			tmp /= 10;
			i++;
		}
		i = 0;
		
		
		
		
	}
		
	return 0;
}

void timer0() interrupt 1
{
	TH0 = (65536 - 10000) / 256;  //10ms
	TL0 = (65536 - 10000) % 256;
	timer0Count++;
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

