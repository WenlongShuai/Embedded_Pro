#ifndef __FUZZY_DECISION_H
#define __FUZZY_DECISION_H

/**
@file       fuzzy_decision.h
@brief      ������ģ���߼��йص����ݽṹ�ͺ����ӿ�
@author     Zev
*/

#include "sys.h"

#define TIME_FUZZY_DELAY 200 // ģ���߼����ߵ�����ʱ�䣬��λΪ����


#define DISTANCE_NEAR 1.5f 	// �ϰ��������ֵ����λΪm
#define DISTANCE_FAR 	2.0f  // �ϰ���Զ����ֵ����λΪm

#define MIN_3(o1, o2, o3) (o1 < (o2 < o3 ? o2 : o3) ? o1 : (o2 < o3 ? o2 : o3)) // ������������Сֵ
#define MAX_2(o1, o2) (o1 < o2 ? o2 : o1)                                       // �������������ֵ
#define SPEED_SUM 2                                                             // �ٶ��ܹ�����������
#define BETA_SUM 5                                                              // �����ܹ����������

//�ļ��в���Ҫʹ��DISTANCE_SAFE������ģ�������ж���ͨ��HC_SR�ļ��е���ֵ
//#define DISTANCE_SAFE 0.4f ///<��ȫ��ֵ����λΪm�����С������ǰ���ϰ���ľ���С��DISTANCE_SAFE���ͻ�ͣ��������ģ������

/**
@brief      ģ�����ߵĽ�����������ҵ�����ٶȣ���λΪdm/s
@param      None
@retval     None
*/
typedef struct
{
	uint8_t fuzzyFlag;   //ģ�����߽����־��0��û���ϰ��1�����ϰ���
	float speed0; ///<Ϊ�˱ܿ��ϰ���ұߵ��Ӧ�ﵽ���ٶȣ���λΪdm/s
	float speed1; ///<Ϊ�˱ܿ��ϰ����ߵ��Ӧ�ﵽ���ٶȣ���λΪdm/s
	uint8_t turnDir;   //ת������
	float turnAngle; //ת���Ƕ�
}FUZZY_ST;

typedef struct
{
	float Code_Speed_Fast;
	float Code_Speed_Slow;
	float Code_Beta_Left;
	float Code_Beta_Left_Front;
	float Code_Beta_Front;
	float Code_Beta_Right_Front;
	float Code_Beta_Right;
}Fuzzy_Centroid;


void fuzzy_Init(void);

FUZZY_ST fuzzy_decision(float dl, float df, float dr);
/**
@}
*/
#endif /* __FUZZY_DECISION_H */
