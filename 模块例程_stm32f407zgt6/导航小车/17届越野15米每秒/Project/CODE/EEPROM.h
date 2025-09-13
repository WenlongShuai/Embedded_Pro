#ifndef _EEPROM_
#define _EEPROM_
#include "headfile.h"
#include "IST8310_IIC.h"
#include "led.h"
#include "GPS.h"
#include "ist8310driver.h"
#include "ist8310driver_middleware.h"
#include <math.h>
#include "TIM.h"
#include "IMU.h"
#include "remote.h"
#include "pwm.h"
#include "Track.h"
void EEPROM_INIT(void);
void EEPROM_TEST(void);
void EEPROM_COLLECT(void);
void EEPROM_READ(void);
void EEPROM_GET_POSI(POSI_ST* posi,uint16 i);
void GET_NUM(uint8* n);
#define EEPROM_ADDR 0X50 //EEPROMµÄIICµØÖ·
#define	EEPROM_DELAY_TIME 10
typedef struct{
	double longitude;
	double latitude;
}WGS84_T;
typedef union{
	uint8 data[40];
	WGS84_T wgs84;
}POSI_UNION;
#endif