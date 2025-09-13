#ifndef __TIMING_FUN_H
#define __TIMING_FUN_H

#include "stm32f4xx.h" 

void Timing_Fun_Init(u16 arr,u16 psc);
void changeTimerPrescaler(uint16_t newPrescaler);

#endif /* __TIMING_FUN_H */
