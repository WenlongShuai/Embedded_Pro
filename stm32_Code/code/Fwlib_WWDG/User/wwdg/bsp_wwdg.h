#ifndef __BSP_WWDG_H__
#define __BSP_WWDG_H__

#include "stm32f10x.h"

#define WWDG_CNT   0x7F

void WWDG_Config(uint8_t prv, uint8_t wr, uint8_t tr);
void WWDG_Feed(void);


#endif  /* __BSP_WWDG_H__ */
