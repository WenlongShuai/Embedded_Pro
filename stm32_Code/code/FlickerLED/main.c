#include "stm32f10x.h"

#define uint unsigned int
#define uchar unsigned char

void delayMs(uint ms);

int main()
{
	//控制PB0引脚
	//打开GPIOB的时钟
	*(uint*)(0x40021000+0x18) |= (1<<3);
	//配置GPIOB的输入输出模式
	*(uint*)0x40010C00 |= ( 1 << (4*0) );
	
	while(1)
	{
		//控制GPIOB输出高低电平
		*(uint*)(0x40010C00+0x0C) &= ~(1<<0);  //亮
		delayMs(1000);
		*(uint*)(0x40010C00+0x0C) |= (1<<0);  //灭
		delayMs(1000);
	}
	
	
	return 0;
}

//延时函数
void delayMs(uint ms)
{
	uint i = 0;
	uint j = 0;
	for(i = 0;i < ms;i++)
		for(j = 0;j < 2003;j++);
}


//因为启动文件会去调用外部的SystemInit函数，为了不让编译器报错，所以先定义一个空的SystemInit函数
void SystemInit()
{

}


