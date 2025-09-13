#ifndef __KEYSCAN_H__
#define __KEYSCAN_H__

#include <reg51.h>
#include <stdio.h>
#include "LCD1602.h"
#include "DS1302.h"


//¶ÀÁ¢°´¼ü
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

extern int rtc[7];

#define uchar unsigned char
#define uint unsigned int

void adjustmentTime(uchar key);
uchar detectKey();


#endif