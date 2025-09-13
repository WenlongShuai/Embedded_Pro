#ifndef __MOTOR_PID_H
#define __MOTOR_PID_H

#include "sys.h"
#include "motor.h"

/**
@brief      PID控制有关的结构体
@details    因为小车只需要控制两个电机，因此只定义了两个结构体变量：pid0和pid1，分别代表小车右边和左边的电机。本程序只用到了PI控制，因此把不需要的变量注释掉了
@note       这是私有结构体
*/
struct PID_ST
{
    int32_t set_point;  ///<设定小车在采样时间内要达到的脉冲数
		int32_t current_point; //小车在采样时间内实时的脉冲数
    int32_t last_error; ///<上一次误差
    //int32_t prev_error;   ///<上上次误差
    float proportion; ///<比例系数
    float integral;   ///<积分系数
                      //float derivative;   ///<微分系数
};


void PID_Config(void);
void PID_Control(int32_t step0, int32_t step1);
void PID_Set_Point(int32_t point0, int32_t point1);

#if defined(DEBUG_PID)
void pid_debug_params(uint32_t pwm, float pd, float ti, float td, MOTOR_DIRECTION dir);
#endif

#if defined(PID_ASSISTANT_EN) 
void set_P_I_D(float p, float i, float d);
void set_PID_Target_A(float temp_val);
#endif

#endif /* __MOTOR_PID_H */
