#include "bsp_iwdg.h"

void IWDG_Config(uint8_t prv, uint16_t rlv)
{
	RCC_LSICmd(ENABLE);  //打开内部低速时钟(LSI)
	
	//启用或禁用对IWDG_PR和IWDG_RLR寄存器的写访问
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//超时时间 Tout = (4*2^prv)/40*rlv (s)
	//Tout = 8/40*5 (s) = 1s
	//预分频寄存器
	IWDG_SetPrescaler(prv);
	
	//重装载寄存器
	IWDG_SetReload(rlv);
	
	//将重装寄存器的值放入计数器中
	IWDG_ReloadCounter();
	
	//启用IWDG(禁用对IWDG_PR和IWDG_RLR寄存器的写访问)
	IWDG_Enable();
}

void IWDG_Feed(void)
{
	//将重装寄存器的值放入计数器中，喂狗操作
	//当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}
