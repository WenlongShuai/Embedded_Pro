#ifndef __BSP_READWRITEINTERFLASH_H__
#define __BSP_READWRITEINTERFLASH_H__

#include "stm32f10x.h"
#include "bsp_usart.h"

#define FLASH_PAGE(n)		((uint32_t)(0x08000000+n*2*1024))

#define WRITE_START_ADDR  FLASH_PAGE(3)
#define WRITE_END_ADDR    FLASH_PAGE(8)
#define PAGE_SIZE					2048

void write_Protect_Enable(void);
void write_Protect_Disable(void);
void read_Protect_Enable(void);
void read_Protect_Disable(void);
void WriteProtect_Toggle(void);
void ReadProtect_Toggle(void);

#endif  /*__BSP_READWRITEINTERFLASH_H__*/
