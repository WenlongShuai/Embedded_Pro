#ifndef _REMOTE_
#define _REMOTE_
#include "mm32_device.h"
#include "hal_conf.h"
#include "struct_typedef.h"
void remote_hander(void);
typedef struct{
	int16_t forword_speed;
	int16_t turn_pwm;
	int16_t mode_pwm;
}REMOTE_DATA_ST;
extern REMOTE_DATA_ST remote_data;
void REMOTE_SHOW(void);
int get_mode(void);
#endif