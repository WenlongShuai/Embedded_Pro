#ifndef __LCD_DEMO_H
#define __LCD_DEMO_H

#include "sys.h"
#include "lcd_ili9341.h"
#include "gui.h"

void Redraw_Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void showimage(const unsigned char *p); 
void QDTFT_Test_Demo(void);

#endif /* __LCD_DEMO_H */
