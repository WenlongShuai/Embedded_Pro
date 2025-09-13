#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include "stm32f10x.h"

/* 使用固件库SysTick实现延时 */
#include "core_cm3.h"

void SysTick_FWLIB_Delay_us(uint32_t us);
void SysTick_FWLIB_Delay_ms(uint32_t ms);


#endif /* __BSP_SYSTICK_H__ */

