#ifndef _FUN_H
#define _FUN_H
#include"sys.h"
#include "pid.h"

__packed typedef struct
{
	double gps_j;
	double gps_w; 
	double wifi_j;
	double wifi_w; 
	float  qmc5883_yaw; 
}SensorDate;


void Get_CurrentVal(double*gps_j,double*gps_w,double*wifi_j,double*wifi_w,float*qmc5883_yaw);
void Calculate_PID(void);
void Navigation_of_Car(void);


#define PWM_OFFSET_L 0
#define PWM_OFFSET_R 0




#endif








