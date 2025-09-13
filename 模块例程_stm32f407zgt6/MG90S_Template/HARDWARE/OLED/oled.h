#ifndef __OLED_H
#define __OLED_H

#include "software_iic.h"


#define OLED_ID          0x3C
#define OLED_WR_CMD      0x00
#define OLED_WR_DATA     0x40

// 定义SSD1306显存缓冲区（128×64像素，每页8行，共8页）
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_PAGES  (OLED_HEIGHT / 8)  // 8页

void OLED_IICTypeInit(void);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_data);
void OLED_Appoint(unsigned char startX, unsigned char startY,unsigned char width, unsigned char height);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char textsize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_UpdateScreen(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



#endif /* __OLED_H */
