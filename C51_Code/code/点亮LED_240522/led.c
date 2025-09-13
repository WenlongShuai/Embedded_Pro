#include <reg51.h>

sbit D1 = P2^0;

int main()
{
	while(1)
	{
		D1 = 1;
	}
	
	return 0;
}