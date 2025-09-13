#ifndef __DS1302_H__
#define __DS1302_H__

#include <reg51.h>

#define uint unsigned int
#define uchar unsigned char


//读取的时间
extern uchar ds1302Read[7];


void DS1302Init();
void readTime();
uchar DS1302ReadByte(uchar readAddr);
void DS1302WriteByte(uchar writeAddr, uchar dat);
void DS1302AdjustmentTime(int *dat);


#endif

