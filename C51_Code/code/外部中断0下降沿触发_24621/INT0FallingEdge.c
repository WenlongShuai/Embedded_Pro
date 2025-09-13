#include <reg51.h>

//K3按键与P3^2相连，每按下一次按键之后，P3^2口就会产生一个下降沿，就会触发外部中断0

int main()
{
	P2 = 0x00;
	//打开总中断
	EA = 1;
	
	//打开中断允许位
	EX0 = 1;
	
	//中断触发方式（低电平触发、下降沿触发）
	IT0 = 1; //下降沿触发
	
	while(1);
	
	return 0;
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

void externalInt0() interrupt 0  
{
	P2++;
	delayMs(500);
}

