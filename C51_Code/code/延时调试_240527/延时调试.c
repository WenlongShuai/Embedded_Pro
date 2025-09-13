#include <reg51.h>

sbit P2_0 = P2^0;

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
	while(1)
	{
		P2_0 = 0;
		
		delayMs(3000);
		
		P2_0 = 1;
		delayMs(3000);

		
	}


	
	return 0;
}