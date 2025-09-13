#ifndef __I2C_H__
#define __I2C_H__

#include <reg51.h>


//I2C �� EEPROM ����ͨ��

sbit EEPROM_SCL = P2^1;
sbit EEPROM_SDA = P2^0;

typedef unsigned char uchar;  //����������
typedef unsigned int uint;

void eepromWriteByte(uchar deviceWriteAddr, uchar wordAddr, uchar dat);
void eepromWritePage(uchar deviceWriteAddr, uchar wordAddr, uchar *writeDat,uchar len);
uchar eepromReadCurrentAddr(uchar deviceReadAddr);
uchar eepromReadRandomAddr(uchar deviceReadAddr, uchar wordAddr);
void eepromReadSequentialAddr(uchar deviceReadAddr, uchar wordAddr,uchar *readDat,uchar readDataLen);
void delay_us(uchar us);
void delayMs(uint ms);


#endif