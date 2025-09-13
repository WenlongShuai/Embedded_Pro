#ifndef __FUZZY_DECISION_H
#define __FUZZY_DECISION_H

/**
@file       fuzzy_decision.h
@brief      定义与模糊逻辑有关的数据结构和函数接口
@author     Zev
*/

#include "sys.h"

#define TIME_FUZZY_DELAY 200 // 模糊逻辑决策的作用时间，单位为毫秒


#define DISTANCE_NEAR 1.5f 	// 障碍物近的阈值，单位为m
#define DISTANCE_FAR 	2.0f  // 障碍物远的阈值，单位为m

#define MIN_3(o1, o2, o3) (o1 < (o2 < o3 ? o2 : o3) ? o1 : (o2 < o3 ? o2 : o3)) // 求三个数的最小值
#define MAX_2(o1, o2) (o1 < o2 ? o2 : o1)                                       // 求两个数的最大值
#define SPEED_SUM 2                                                             // 速度总共有两个形心
#define BETA_SUM 5                                                              // 方向总共有五个形心

//文件中不需要使用DISTANCE_SAFE，进行模糊决策判断是通过HC_SR文件中的阈值
//#define DISTANCE_SAFE 0.4f ///<安全阈值，单位为m，如果小车发现前方障碍物的距离小于DISTANCE_SAFE，就会停下来进行模糊决策

/**
@brief      模糊决策的结果，包含左右电机的速度，单位为dm/s
@param      None
@retval     None
*/
typedef struct
{
	uint8_t fuzzyFlag;   //模糊决策结果标志，0：没有障碍物，1：有障碍物
	float speed0; ///<为了避开障碍物，右边电机应达到的速度，单位为dm/s
	float speed1; ///<为了避开障碍物，左边电机应达到的速度，单位为dm/s
	uint8_t turnDir;   //转动方向
	float turnAngle; //转动角度
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
