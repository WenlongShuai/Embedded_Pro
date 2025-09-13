#include <reg51.h>

sbit D1 = P2^0;
sbit buzzer = P1^5;

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}

void ledBuzzer()
{
	D1 = ~D1;
	buzzer = ~buzzer;
}

int main()
{
	while(1)
	{
		ledBuzzer();
		delayMs(1000);
	}
	return 0;
}