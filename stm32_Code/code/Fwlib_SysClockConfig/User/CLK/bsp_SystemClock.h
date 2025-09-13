#ifndef __BSP_SYSTEMCLOCK_H__
#define __BSP_SYSTEMCLOCK_H__

#include "stm32f10x.h"

void HSE_SetSysClock_REG(uint32_t RCC_CFGR_PLLMULLx);
void HSI_SetSysClock_REG(uint32_t RCC_CFGR_PLLMULLx);

void HSE_SysSetClock_FWLib(uint32_t RCC_PLLMul_x);
void HSI_SysSetClock_FWLib(uint32_t RCC_PLLMul_x);

#endif /*__BSP_SYSTEMCLOCK_H__*/
