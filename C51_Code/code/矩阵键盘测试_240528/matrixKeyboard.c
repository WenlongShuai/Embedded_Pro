#include <reg51.h>


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
	int temp = 0;
	int num = 0;
	//因为P1是准双向，作为输入io口时要先拉高再使用，作为输出io时直接使用。
	P1 = 0xff;   //矩阵键盘
	P2 = 0x04;   //数码管位选
	P0 = 0x00;   //数码管段选
	while(1)
	{
		P1 = 0x7f;
		temp = 0xff;
		temp &= P1;
		if(temp != 0x7f)  //有键按下
		{
			delayMs(5); //按下消抖处理
			while(temp == 0x7f);
			switch(temp)
			{
				case 0x77:
					num = 0;
					break;
				case 0x7b:
					num = 1;
					break;
				case 0x7d:
					num = 2;
					break;
				case 0x7e:
					num = 3;
					break;
			}
			
			delayMs(5);  //释放消抖处理
			while(temp == 0x7f);
			
			//按下什么键，数码管显示什么数字
			P0 = table[num];
		}
		
		P1 = 0xbf;
		temp = 0xff;
		temp &= P1;
		if(temp != 0xbf)  //有键按下
		{
			delayMs(5); //按下消抖处理
			while(temp == 0xbf);
			switch(temp)
			{			
				case 0xb7:
					num = 4;
					break;
				case 0xbb:
					num = 5;
					break;
				case 0xbd:
					num = 6;
					break;
				case 0xbe:
					num = 7;
					break;
			}
			
			delayMs(5);  //释放消抖处理
			while(temp == 0xbf);
			
			//按下什么键，数码管显示什么数字
			P0 = table[num];
		}
		
		P1 = 0xdf;
		temp = 0xff;
		temp &= P1;
		if(temp != 0xdf)  //有键按下
		{
			delayMs(5); //按下消抖处理
			while(temp == 0xdf);
			switch(temp)
			{	
				case 0xd7:
					num = 8;
					break;
				case 0xdb:
					num = 9;
					break;
				case 0xdd:
					num = 10;
					break;
				case 0xde:
					num = 11;
					break;
			}
			
			delayMs(5);  //释放消抖处理
			while(temp == 0xdf);
			
			//按下什么键，数码管显示什么数字
			P0 = table[num];
		}
	
		
		P1 = 0xef;
		temp = 0xff;
		temp &= P1;
		if(temp != 0xef)  //有键按下
		{
			delayMs(5); //按下消抖处理
			while(temp == 0xef);
			switch(temp)
			{
				case 0xe7:
					num = 12;
					break;
				case 0xeb:
					num = 13;
					break;
				case 0xed:
					num = 14;
					break;
				case 0xee:
					num = 15;
					break;
			}
			
			delayMs(5);  //释放消抖处理
			while(temp == 0xef);
			
			//按下什么键，数码管显示什么数字
			P0 = table[num];
		}
		
		
		if(temp == 0xff)
		{
			P0 = 0x00;
		}
	
	}
	return 0;
}