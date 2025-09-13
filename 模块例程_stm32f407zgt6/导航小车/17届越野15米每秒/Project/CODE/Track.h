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
#include "user.h"
#include "EEPROM.h"
#define PI 3.1415926535898
void Track_Show(void);
void Track(void);
void Track_INIT(void);
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour);
