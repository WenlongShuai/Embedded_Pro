#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

//BitBand
//ODR寄存器地址映射
#define  GPIOA_ODR_ADDR (GPIOA_BASE+0x0C)
#define  GPIOB_ODR_ADDR (GPIOB_BASE+0x0C)
#define  GPIOC_ODR_ADDR (GPIOC_BASE+0x0C)
#define  GPIOD_ODR_ADDR (GPIOD_BASE+0x0C)
#define  GPIOE_ODR_ADDR (GPIOE_BASE+0x0C)
#define  GPIOF_ODR_ADDR (GPIOF_BASE+0x0C)
#define  GPIOG_ODR_ADDR (GPIOG_BASE+0x0C)
											
//IDR寄存器地址映射      
#define  GPIOA_IDR_ADDR (GPIOA_BASE+0x08)
#define  GPIOB_IDR_ADDR (GPIOB_BASE+0x08)
#define  GPIOC_IDR_ADDR (GPIOC_BASE+0x08)
#define  GPIOD_IDR_ADDR (GPIOD_BASE+0x08)
#define  GPIOE_IDR_ADDR (GPIOE_BASE+0x08)
#define  GPIOF_IDR_ADDR (GPIOF_BASE+0x08)
#define  GPIOG_IDR_ADDR (GPIOG_BASE+0x08)

//输出
#define GPIOA_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOA_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOB_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOB_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOC_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOC_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOD_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOD_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOE_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOE_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOF_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOF_ODR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOG_ODR_OUT(n) *(unsigned int *)(0x42000000+(GPIOG_ODR_ADDR - 0x40000000)*8*4+n*4)

//输入
#define GPIOA_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOA_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOB_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOB_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOC_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOC_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOD_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOD_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOE_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOE_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOF_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOF_IDR_ADDR - 0x40000000)*8*4+n*4)
#define GPIOG_IDR_IN(n) *(unsigned int *)(0x42000000+(GPIOG_IDR_ADDR - 0x40000000)*8*4+n*4)


void Delay(uint32_t time);

int main()
{
	uint8_t key = 0;
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	#if 0
	//使用位带操作控制LED灯
	while(1)
	{
		#if 0
		GPIOB_ODR_OUT(0) = 0;
		Delay(0x5FFFFF);
		GPIOB_ODR_OUT(0) = 1;
		Delay(0x5FFFFF);
		
		#elif 0
		GPIOB_ODR_OUT(1) = 0;
		Delay(0x5FFFFF);
		GPIOB_ODR_OUT(1) = 1;
		Delay(0x5FFFFF);
		
		#elif 1
		GPIOB_ODR_OUT(5) = 0;
		Delay(0x5FFFFF);
		GPIOB_ODR_OUT(5) = 1;
		Delay(0x5FFFFF);
		
		#endif
	}
	
	#elif 1
	//使用位带操作检测按键，并控制LED灯状态
	while(1)
	{
		key = GPIOA_IDR_IN(0);
		if(key == KEY_ON)
		{
			while(GPIOA_IDR_IN(0) == KEY_ON);
			LED_G_TOGGLE;
		}
		
		key = GPIOC_IDR_IN(13);
		if(key == KEY_ON)
		{
			while(GPIOC_IDR_IN(13) == KEY_ON);
			LED_R_TOGGLE;
		}
	}
	
	#endif
	return 0;
}

void Delay(uint32_t time)
{
	while(time--);
}

