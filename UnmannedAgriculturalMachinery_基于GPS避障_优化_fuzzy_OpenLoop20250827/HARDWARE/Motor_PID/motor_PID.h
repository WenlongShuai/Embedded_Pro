#ifndef __MOTOR_PID_H
#define __MOTOR_PID_H

#include "sys.h"
#include "motor.h"

/**
@brief      PID�����йصĽṹ��
@details    ��ΪС��ֻ��Ҫ����������������ֻ�����������ṹ�������pid0��pid1���ֱ����С���ұߺ���ߵĵ����������ֻ�õ���PI���ƣ���˰Ѳ���Ҫ�ı���ע�͵���
@note       ����˽�нṹ��
*/
struct PID_ST
{
    int32_t set_point;  ///<�趨С���ڲ���ʱ����Ҫ�ﵽ��������
		int32_t current_point; //С���ڲ���ʱ����ʵʱ��������
    int32_t last_error; ///<��һ�����
    //int32_t prev_error;   ///<���ϴ����
    float proportion; ///<����ϵ��
    float integral;   ///<����ϵ��
                      //float derivative;   ///<΢��ϵ��
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
