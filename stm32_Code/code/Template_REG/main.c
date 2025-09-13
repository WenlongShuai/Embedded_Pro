#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define GPIO_CLOCK_ENABLE (RCC->APB2ENR |= (1<<3))
#define GPIO_PIN          GPIO_Pin_5
#define GPIO_PORT         GPIOB

int main()
{
	#if 0
	//使用外设基地址访问寄存器
	//配置GPIOB的时钟
	RCC_APB2ENR |= (1<<3);
	//配置GPIOB的输入输出模式
	GPIOB_CRL &= ~(0xf<<(4*5));  //将CRL寄存器中的PB5清零
	GPIOB_CRL |= (1<<(4*5));  //将CRL寄存器中的PB5设置为0001
	//控制GPIOB输出高低电平
	GPIOB_ODR &= ~(1<<5);
	
	#elif 0
	
	//使用结构体访问外设寄存器
	//配置GPIOB的时钟
	RCC->APB2ENR |= (1<<3);
	//配置GPIOB的输入输出模式
	GPIOB->CRL &= ~(0xf<<(4*5));  //将CRL寄存器中的PB5清零
	GPIOB->CRL |= (1<<(4*5));  //将CRL寄存器中的PB5设置为0001
	//控制GPIOB输出高低电平
	GPIOB->ODR &= ~(1<<5);
	
	#elif 0
	
	//提高可读性
	GPIO_InitStructure_Typedef GPIO_Struct;
	//配置GPIOB的时钟
	RCC->APB2ENR |= (1<<3);
	
	//RED
	//配置GPIOB的输入输出模式
	GPIO_Struct.GPIO_Pinx = GPIO_Pin_5;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHZ;
	
	GPIO_Init(GPIOB, &GPIO_Struct);
	//使用BRR寄存器输出低电平
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	//使用BSRR寄存器输出高电平
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	
	//GREEN
	GPIO_Struct.GPIO_Pinx = GPIO_Pin_0;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHZ;
	
	GPIO_Init(GPIOB, &GPIO_Struct);
	//使用BRR寄存器输出低电平
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	//使用BSRR寄存器输出高电平
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	
	//BLUE
	GPIO_Struct.GPIO_Pinx = GPIO_Pin_1;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHZ;
	
	GPIO_Init(GPIOB, &GPIO_Struct);
	//使用BRR寄存器输出低电平
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	//使用BSRR寄存器输出高电平
	//GPIO_SetBits(GPIOB, GPIO_Pin_1);
	
	#elif 1
	
	//提高移植性
	GPIO_InitStructure_Typedef GPIO_Struct;
	//配置GPIOB的时钟
	GPIO_CLOCK_ENABLE;
	
	//RED
	//配置GPIOB的输入输出模式
	GPIO_Struct.GPIO_Pinx = GPIO_PIN;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHZ;
	
	GPIO_Init(GPIO_PORT, &GPIO_Struct);
	//使用BRR寄存器输出低电平
	GPIO_ResetBits(GPIO_PORT, GPIO_PIN);
	//使用BSRR寄存器输出高电平
	//GPIO_SetBits(GPIO_PORT, GPIO_PIN);
	
	
	
	#endif
	
	return 0;
}

//因为启动文件会去调用外部的SystemInit函数，为了不让编译器报错，所以先定义一个空的SystemInit函数
void SystemInit()
{

}
