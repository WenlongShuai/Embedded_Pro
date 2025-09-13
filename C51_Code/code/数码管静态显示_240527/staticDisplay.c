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
	int i = 0;
	P2 = 0xff;  //数码管的位选，选择最后一个数码管
	
	while(1)
	{
		for(i = 0;i < 16;i++)
		{
			P0 = table[i];
			delayMs(1000);
		}
	}
	
	return 0;
}