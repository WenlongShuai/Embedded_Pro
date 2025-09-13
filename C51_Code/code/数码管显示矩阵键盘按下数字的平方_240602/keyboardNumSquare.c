#include <reg51.h>

//按下16个矩阵键盘依次在数码管上显示1-16平方的值

char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x77, 0x7c,
										0x39, 0x5e, 0x79, 0x71};

void delayMs(unsigned int ms);
int keyboardScanf();
										
int main()
{
	int keyNum = 0;  //键值
	int n = 0;  //保存矩阵键盘上一个状态的键值
	int mul = 0;
	unsigned char num = 0;
	P1 = 0xFF;

	while(1)
	{
		keyNum = keyboardScanf();
		if(keyNum == 0)
		{
			keyNum = n;
		}
		else
		{
			n = keyNum;
		}
		
		P2 = 0x00;
		mul = keyNum * keyNum;
		while(mul)
		{
			num = mul % 10;
			mul /= 10;
			P2 = P2;
			P0 = table[num];
	
			delayMs(1);	
			P0 = 0x00;
			P2 += 0x04;	
		}	

	}
	return 0;
}

int keyboardScanf()
{
	int num = 0;
	P1 = 0x7F;  //第一行
	if(P1 != 0x7F)
	{
		delayMs(5);
		while(P1 == 0x7F);
		
		switch(P1)
		{
			case 0x77:num = 1;break;
			case 0x7B:num = 2;break;
			case 0x7D:num = 3;break;
			case 0x7E:num = 4;break;
		}
		delayMs(5);
		while(P1 != 0x7F);
	}
	
	P1 = 0xBF;  //第二行
	if(P1 != 0xBF)
	{
		delayMs(5);
		while(P1 == 0xBF);
		
		switch(P1)
		{
			case 0xB7:num = 5;break;
			case 0xBB:num = 6;break;
			case 0xBD:num = 7;break;
			case 0xBE:num = 8;break;
		}
		delayMs(5);
		while(P1 != 0xBF);
	}
	
	P1 = 0xDF;  //第三行
	if(P1 != 0xDF)
	{
		delayMs(5);
		while(P1 == 0xDF);
		
		switch(P1)
		{
			case 0xD7:num = 9;break;
			case 0xDB:num = 10;break;
			case 0xDD:num = 11;break;
			case 0xDE:num = 12;break;
		}
		delayMs(5);
		while(P1 != 0xDF);
	}
	
	
	P1 = 0xEF;  //第四行
	if(P1 != 0xEF)
	{
		delayMs(5);
		while(P1 == 0xEF);
		
		switch(P1)
		{
			case 0xE7:num = 13;break;
			case 0xEB:num = 14;break;
			case 0xED:num = 15;break;
			case 0xEE:num = 16;break;
		}
		delayMs(5);
		while(P1 != 0xEF);
	}

	return num;
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
	