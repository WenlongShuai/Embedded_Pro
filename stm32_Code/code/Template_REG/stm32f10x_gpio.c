#include "stm32f10x_gpio.h"

//GPIO置位函数，配置BSRR
void GPIO_SetBits(GPIO_Typedef *GPIOx, unsigned short GPIO_Pin)
{
	GPIOx->BSRR |= GPIO_Pin;
}

//GPIO复位函数，配置BRR
void GPIO_ResetBits(GPIO_Typedef *GPIOx, unsigned short GPIO_Pin)
{
	GPIOx->BRR |= GPIO_Pin;
}

//GPIO初始化函数，配置CRL
void GPIO_Init(GPIO_Typedef *GPIOx, GPIO_InitStructure_Typedef *GPIO_Struct)
{
	unsigned char speed = 0x00;
	unsigned int tmpCRL = 0x00, tmpCRH = 0x00;
	unsigned char pin = 0;
	unsigned int mode = 0x00;
	unsigned char i = 0;
	
	//判断GPIO的模式是输入模式还是输出模式，判断GPIO_Mode的bit4是0还是1
	if(((GPIO_Struct->GPIO_Mode) & 0x10) != 0x00)
	{
		speed = GPIO_Struct->GPIO_Speed;
	}
	
	//判断GPIO_Pinx是否为低8位
	if((GPIO_Struct->GPIO_Pinx & 0xFF00) == 0)
	{
		tmpCRL = GPIOx->CRL;
		//确定GPIO具体的引脚
		for(i = 0;i < 8;i++)
		{
			if((GPIO_Struct->GPIO_Pinx & (1<<i)) == GPIO_Struct->GPIO_Pinx)
			{
				pin = i;
				break;
			}
		}
		
		//对CRL清零操作
		tmpCRL &= ~(0x0f<<(4*pin));
		
		//设置CRL的值
		mode = GPIO_Struct->GPIO_Mode & 0x0F;
		tmpCRL |= ((mode | speed) << (4*pin));
		
		if(GPIO_Struct->GPIO_Mode == GPIO_Mode_IPD)
		{
			GPIOx->BRR |= (1<<pin);
		}
		else
		{
			GPIOx->BSRR |= (1<<pin);
		}
		
		GPIOx->CRL = tmpCRL;
	}
	else
	{
		tmpCRH = GPIOx->CRH;
		//确定GPIO具体的引脚
		for(i = 8;i < 16;i++)
		{
			if(GPIO_Struct->GPIO_Pinx & (1<<i) != 0x00)
			{
				pin = i;
				break;
			}
		}
		
		//对CRH清零操作
		tmpCRH &= ~(0x0f<<(4*pin));
		
		//设置CRL的值
		mode = GPIO_Struct->GPIO_Mode & 0x0F;
		tmpCRH |= (mode | speed) << (4*pin);
		
		if(GPIO_Struct->GPIO_Mode == GPIO_Mode_IPD)
		{
			GPIOx->BRR |= (1<<pin);
		}
		else
		{
			GPIOx->BSRR |= (1<<pin);
		}
		
		GPIOx->CRH = tmpCRH;
	}
}

