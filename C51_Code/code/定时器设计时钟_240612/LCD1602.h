#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <reg51.h>

sbit lcd1602_RS = P2^6;
sbit lcd1602_WR = P2^5;
sbit lcd1602_EN = P2^7;

typedef unsigned int uint;
typedef unsigned char uchar;

void lcd1602Init();
uchar lcd1602ReadState();
void lcd1602WriteCommand(uchar cmd);
void lcd1602WriteData(uchar dat);
uchar lcd1602ReadData();
void lcdDisplayChar(unsigned char x, unsigned char y, unsigned char dat);
void lcdDisplatString(unsigned char x, unsigned char y, char *str);
void lcd1602Clear();
void delayMs(unsigned int ms);

#endif