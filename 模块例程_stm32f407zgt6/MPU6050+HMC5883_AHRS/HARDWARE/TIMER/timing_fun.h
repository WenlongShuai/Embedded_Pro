#ifndef __TIMING_FUN_H
#define __TIMING_FUN_H

#include "stm32f10x.h"
 
 
/*  常用参数 */
#define PI   3.14f
#define DEG2RAD	   PI/180

void Timing_Fun_Init(u16 arr,u16 psc);
void changeTimerPrescaler(uint16_t newPrescaler);

#endif /* __TIMING_FUN_H */
