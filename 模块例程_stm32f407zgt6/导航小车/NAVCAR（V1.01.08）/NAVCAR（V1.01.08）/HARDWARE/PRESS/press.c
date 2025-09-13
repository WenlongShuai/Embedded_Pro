#include "myiic.h"
#include "press.h"
#include "delay.h"

long Pa;

#define BMP280_ADDR         0xec
#define BMP280_TEMP_ADDR    0xfa
#define BMP280_PRESS_ADDR   0xf7

unsigned short dig_T1;
short dig_T2;
short dig_T3;
unsigned short dig_P1;
short dig_P2;
short dig_P3;
short dig_P4;
short dig_P5;
short dig_P6;
short dig_P7;
short dig_P8;
short dig_P9;


#define FOSC    11059200UL
#define BRT     (256 - FOSC / 9600 / 32)

char bufferPtr;
char Uart1Buffer[16];




//void I2C_Wait()
//{
//    while(!(I2CMSST & 0x40));
//        //Uart1SendStr("i2c wait...\r\n");
//    I2CMSST &= ~0x40;
//}

//void I2C_Start()
//{
//    I2CMSCR = 0x01;
//    I2C_Wait();
//}

//void I2C_SendData(char dat)
//{
//    I2CTXD = dat;
//    I2CMSCR = 0x02;
//    I2C_Wait();
//}

//void I2C_RecvACK()
//{
//    I2CMSCR = 0x03;
//    I2C_Wait();
//}

//char I2C_RecvData()
//{
//    I2CMSCR = 0x04;
//    I2C_Wait();
//    return(I2CRXD);
//}

//void I2C_SendACK()
//{
//    I2CMSST = 0x00;
//    I2CMSCR = 0x05;
//    I2C_Wait();
//}

//void I2C_SendNAK()
//{
//    I2CMSST = 0x01;
//    I2CMSCR = 0x05;
//    I2C_Wait();
//}

//void I2C_Stop()
//{
//    I2CMSCR = 0x06;
//    I2C_Wait();
//}

//void I2C_Init()
//{
//    P_SW2 = 0x80;
//    I2CCFG = 0xe0;
//    I2CMSST = 0x00;
//}



unsigned char bmp280_ReadByte(unsigned char addr)
{
    unsigned char temp;

    IIC_Start();
    IIC_Send_Byte(BMP280_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();

    IIC_Start();
    IIC_Send_Byte(BMP280_ADDR + 1);
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();

    return temp;
}

void bmp280_WriteByte(unsigned char addr, unsigned char dat)
{
    IIC_Start();
    IIC_Send_Byte(BMP280_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
    IIC_Send_Byte(dat);
    IIC_Wait_Ack();
    IIC_Stop();
	delay_ms(10);
}

long bmp280_MultipleReadThree(unsigned char addr)
{
    unsigned char msb, lsb, xlsb;
    long temp = 0;
    msb = bmp280_ReadByte(addr);
    lsb = bmp280_ReadByte(addr + 1);
    xlsb = bmp280_ReadByte(addr + 2);

    temp = (long)(((unsigned long)msb << 12)|((unsigned long)lsb << 4)|((unsigned long)xlsb >> 4));

    return temp;
}

short bmp280_MultipleReadTwo(unsigned char addr)
{
    unsigned char msb, lsb;
    short temp = 0;
    lsb = bmp280_ReadByte(addr);
    msb = bmp280_ReadByte(addr + 1);

    temp = (short)msb << 8;
    temp |= (short)lsb;

    return temp;
}

void bmp280_Init(void)
{
    unsigned char temp = 0;

    //状态全部清零
    bmp280_WriteByte(0xe0, 0xb6);

    //读取ID的时候不知道为啥读不出来了，索性跳过去了
//  temp = bmp280_ReadByte(0xd0);
//  if(temp == 0x58)
//      Uart1SendStr("bmp280 id is right...\r\n");
//  else
//      Uart1SendStr("bmp280 id is error...\r\n");

    bmp280_WriteByte(0xf4, 0xff);
    bmp280_WriteByte(0xf5, 0x00);

    dig_T1 = bmp280_MultipleReadTwo(0x88);
    dig_T2 = bmp280_MultipleReadTwo(0x8A);
    dig_T3 = bmp280_MultipleReadTwo(0x8C);
    dig_P1 = bmp280_MultipleReadTwo(0x8E);
    dig_P2 = bmp280_MultipleReadTwo(0x90);
    dig_P3 = bmp280_MultipleReadTwo(0x92);
    dig_P4 = bmp280_MultipleReadTwo(0x94);
    dig_P5 = bmp280_MultipleReadTwo(0x96);
    dig_P6 = bmp280_MultipleReadTwo(0x98);
    dig_P7 = bmp280_MultipleReadTwo(0x9A);
    dig_P8 = bmp280_MultipleReadTwo(0x9C);
    dig_P9 = bmp280_MultipleReadTwo(0x9E);

    delay_ms(200);        
}

//  dig_T1 = 27504;
//  dig_T2 = 26435;
//  dig_T3 = -1000;
//  dig_P1 = 36477;
//  dig_P2 = -10685;
//  dig_P3 = 3024;
//  dig_P4 = 2855;
//  dig_P5 = 140;
//  dig_P6 = -7;
//  dig_P7 = 15500;
//  dig_P8 = -14600;
//  dig_P9 = 6000;
//  adc_T = 519888;
//  adc_P = 415148;

long bmp280_GetValue(void)
{
    long adc_T;
    long adc_P;
    long var1, var2, t_fine, T, p;

    adc_T = bmp280_MultipleReadThree(BMP280_TEMP_ADDR);
    adc_P = bmp280_MultipleReadThree(BMP280_PRESS_ADDR);

    if(adc_P == 0)
    {
        return 0;
    }

    //Temperature
    var1 = (((double)adc_T)/16384.0-((double)dig_T1)/1024.0)*((double)dig_T2);
    var2 = ((((double)adc_T)/131072.0-((double)dig_T1)/8192.0)*(((double)adc_T)
                /131072.0-((double)dig_T1)/8192.0))*((double)dig_T3);

    t_fine = (unsigned long)(var1+var2);

    T = (var1+var2)/5120.0;

    var1 = ((double)t_fine/2.0)-64000.0;
    var2 = var1*var1*((double)dig_P6)/32768.0;
    var2 = var2 +var1*((double)dig_P5)*2.0;
    var2 = (var2/4.0)+(((double)dig_P4)*65536.0);
    var1 = (((double)dig_P3)*var1*var1/524288.0+((double)dig_P2)*var1)/524288.0;
    var1 = (1.0+var1/32768.0)*((double)dig_P1);
    p = 1048576.0-(double)adc_P;
    p = (p-(var2/4096.0))*6250.0/var1;
    var1 = ((double)dig_P9)*p*p/2147483648.0;
    var2 = p*((double)dig_P8)/32768.0;
    p = p+(var1+var2+((double)dig_P7))/16.0;

    return p;
}

