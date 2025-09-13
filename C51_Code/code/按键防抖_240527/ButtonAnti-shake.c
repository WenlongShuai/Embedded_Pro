#include <reg51.h>

sbit d1 = P2^0;
sbit key1 = P3^1;

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
	int num = 0;
	
	P2 = 0xE3;  //数码管位选，选择第一个数码管

	while(1)
	{
		if(key1 == 0)  //先判断按键是否按下
		{
			delayMs(5);  //按下消抖，延时5ms
			if(key1 == 0)  //再次检测按键是否真正按下
			{
				d1 = 0;
				P0 = table[num];  //数码管段选
				num++;
				if(num == 10)
				{
					num = 0;
				}
				while(key1 == 0);  //判断按键是否处于按下状态
				delayMs(5);  //释放消抖，延时5ms
				while(key1 == 0);  //再次判断是否处于按下状态
			}
		}
		else
		{
			d1 = 1;
		}
	}
	return 0;
}

