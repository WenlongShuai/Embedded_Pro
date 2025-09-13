#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include "stm32f10x.h"

#define SYSTICK_REG   0
#define SYSTICK_FWLIB 1
#define SYSTICK_INT   0

#if SYSTICK_REG
/* 操作寄存器SysTick实现延时 */
#define SYSTICK_BASE 	((uint32_t)0xE000E010)  //SysTick寄存器基地址
#define SYSTICK_CTRL 	*(unsigned int*)(SYSTICK_BASE+0x00)
#define SYSTICK_LOAD 	*(unsigned int*)(SYSTICK_BASE+0x04)
#define SYSTICK_VAL  	*(unsigned int*)(SYSTICK_BASE+0x08)
#define SYSTICK_CALIB *(unsigned int*)(SYSTICK_BASE+0x0C)

#elif SYSTICK_FWLIB
/* 使用固件库SysTick实现延时 */
#include "core_cm3.h"

void SysTick_FWLIB_Delay_us(uint32_t us);
void SysTick_FWLIB_Delay_ms(uint32_t ms);

#elif SYSTICK_INT
/* 使用SysTick中断实现延时 */
extern uint32_t delayTime;

void SysTick_INT_Delay_us(uint32_t us);
void SysTick_INT_Delay_ms(uint32_t ms);


#endif

#if SYSTICK_REG
void SysTick_REG_Delay_us(uint32_t us);
void SysTick_REG_Delay_ms(uint32_t ms);

#elif SYSTICK_FWLIB
/* 使用固件库SysTick实现延时 */


#elif SYSTICK_INT
/* 使用SysTick中断实现延时 */

#endif
	


#endif /* __BSP_SYSTICK_H__ */

