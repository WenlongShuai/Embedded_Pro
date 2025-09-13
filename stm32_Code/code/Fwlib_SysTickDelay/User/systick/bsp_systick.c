#include "bsp_systick.h"

#if SYSTICK_REG

static void SysTick_REG_Config(uint32_t loadValue)
{
	SYSTICK_LOAD = loadValue;   //初值设置为72   延时1us   72000 延时1ms
	SYSTICK_VAL = 0;   //清零当前计数器的值
	SYSTICK_CTRL = SysTick_CTRL_ENABLE |
								 SysTick_CTRL_CLKSOURCE | 
								 SysTick_CTRL_TICKINT;     //选择时钟源、systick递减到0之后产生异常中断、使能SysTick
}

void SysTick_REG_Delay_us(uint32_t us)
{
	uint32_t i = 0;
	SysTick_REG_Config(72);  //us
	for(i = 0;i < us;i++)
	{
		while(!(SYSTICK_CTRL & SysTick_CTRL_COUNTFLAG));  //判断是否递减到0，如果是，则SysTick_CTRL_COUNTFLAG为1，否则为0
	}
	SYSTICK_CTRL &= ~SysTick_CTRL_ENABLE;   //停止SysTick定时计数
}

void SysTick_REG_Delay_ms(uint32_t ms)
{
	uint32_t i = 0;
	SysTick_REG_Config(72000);  //ms
	for(i = 0;i < ms;i++)
	{
		while(!(SYSTICK_CTRL & SysTick_CTRL_COUNTFLAG));
	}
	SYSTICK_CTRL &= ~SysTick_CTRL_ENABLE;
}

#elif SYSTICK_FWLIB
void SysTick_FWLIB_Delay_us(uint32_t us)
{
	uint32_t i = 0;
	SysTick_Config(72);   //us
	for(i = 0;i < us;i++)
	{
		while(!(SysTick->CTRL & (1<<16)));  //判断是否递减到0，如果是，则SysTick_CTRL_COUNTFLAG为1，否则为0
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   //停止SysTick定时计数
}

void SysTick_FWLIB_Delay_ms(uint32_t ms)
{
	uint32_t i = 0;
	SysTick_Config(72000);  //ms
	for(i = 0;i < ms;i++)
	{
		while(!(SysTick->CTRL & (1<<16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

#elif SYSTICK_INT
uint32_t delayTime = 0;

void SysTick_INT_Delay_us(uint32_t us)
{
	delayTime = us;
	SysTick_Config(72);   //us
	while(delayTime);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_INT_Delay_ms(uint32_t ms)
{
	delayTime = ms;
	SysTick_Config(72000);  //ms
	while(delayTime);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

#endif
