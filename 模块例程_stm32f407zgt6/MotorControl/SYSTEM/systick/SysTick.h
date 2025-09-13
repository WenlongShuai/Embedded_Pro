#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);
void Delay_ms(__IO u32 nTime); 	 //µ¥Î»ms
int get_tick_count(unsigned long *count);

#endif /* __SYSTICK_H */
