#include <reg51.h>
#include <intrins.h>


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
	//方法一
//	int num = 0xFF;
//	int count = 0;
//	while(1)
//	{
//		if(count > 8)
//		{
//			count = 0;
//		}
//		P2 = num & ~(1<<count);
//		count++;
//		delayMs(100);
//	}
	
	//方法二
	P2 = 0xFE;
	while(1)
	{
		P2 = _crol_(P2, 1);
		delayMs(100);
	}
	
	return 0;
}