#ifndef __WIFI_H
#define __WIFI_H	 
#include "sys.h"



void ESP8266_Init(void);
void ESP8266_SendDate(char *date);
void Get_Value_From_WIFI(unsigned char* ucFlag,double *JD,double *WD);


#endif

 



