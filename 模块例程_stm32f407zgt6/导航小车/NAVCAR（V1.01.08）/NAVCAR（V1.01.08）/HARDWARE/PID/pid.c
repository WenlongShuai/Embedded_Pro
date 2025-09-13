#include "pid.h"
#include "usart.h"


void PID_Init(struct PID *pid,float kp,float ki,float kd,float LimitMax,float LimitMin)
{
	pid->SetValue=0.0;
	pid->ActualValue=0.0;
	pid->Error=0.0;
	pid->Error1=0.0;
	pid->Error2=0.0;
	
	pid->KP=kp;
	pid->KI=ki;
	pid->KD=kd;
	
	pid->OutMax=LimitMax;
	pid->OutMin=LimitMin;
	pid->OUT=0.0;
}
 

void PID_Control(struct PID *pid)
{
	float DeltaOut=0.0;
	
	pid->Error=pid->SetValue-pid->ActualValue;
	
	//printf("pid->ErrorÎª£º%.2f\r\n",pid->Error);
	DeltaOut=pid->KP*(pid->Error-pid->Error1)+pid->KI*pid->Error+pid->KD*(pid->Error-2*pid->Error1+pid->Error2);
	//printf("DeltaOutÎª£º%.2f\r\n",DeltaOut);
	pid->OUT+=DeltaOut;
	//printf("pid->OUTÎª£º%.2f\r\n",pid->OUT);
	if(pid->OUT >= pid->OutMax){pid->OUT = pid->OutMax;}
	if(pid->OUT <= pid->OutMin){pid->OUT = pid->OutMin;}
	
	pid->Error2=pid->Error1;
	pid->Error1=pid->Error;	
}


