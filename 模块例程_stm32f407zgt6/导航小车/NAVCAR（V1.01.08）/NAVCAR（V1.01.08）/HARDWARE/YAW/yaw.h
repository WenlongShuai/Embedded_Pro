#ifndef __YAW_H
#define __YAW_H	 
#include "sys.h"

typedef struct 
{
	float yaw;
}yaw_msg;






void QMC_5883L_Init(void);
void YAW_Analysis(yaw_msg* yaw_var,u8 *buf_var);




#endif

 



