#ifndef _PID_H
#define _PID_H

struct PID
{
	double SetValue; 
	double ActualValue;
	double Error;
	double Error1;
	double Error2;
	float KP; 
	float KI; 
	float KD;
	float OutMax; 
	float OutMin; 
	float OUT; 
};

void PID_Init(struct PID *pid,float kp,float ki,float kd,float LimitMax,float LimitMin);
void PID_Control(struct PID *pid);

#endif








