/**
@file       fuzzy_decision.c
@brief      实现与模糊逻辑有关的函数 
*/

#include "fuzzy_decision.h"

#include "fun.h"

extern float RC_Velocity;
extern float adjustedFWValue;
extern float currentFWValue;

static float distance[3][2];                                                                                                        // 第一维依次表示左边障碍物的距离,前方障碍物的距离,右边障碍物的距离，第二维依次表示远，近
static float v[SPEED_SUM];                                                                                                          // 总共有两位，依次表示速度快，速度慢
static float beta[BETA_SUM];                                                                                                        // 依次表示正左，左前，正前，右前，正右
static float beta_core[BETA_SUM] = {0.0f}; // 为了方便编程
float speed_core[SPEED_SUM] = {0.0f};  		 // 为了方便编程

static void fuzzifier(const float *fp);
static FUZZY_ST defuzzifier(void);
static Fuzzy_Centroid FuzzyCentroidStruct;

void fuzzy_Init(void)
{
	FuzzyCentroidStruct.Code_Speed_Fast = 0;                         // 速度快的形心，单位为dm/s
	FuzzyCentroidStruct.Code_Speed_Slow = 0;                         // 速度慢的形心，单位为dm/s
	FuzzyCentroidStruct.Code_Beta_Left = -(0.5f * PI);               // 正左方向的形心，单位为弧度（-45°）
	FuzzyCentroidStruct.Code_Beta_Left_Front = -(0.25f * PI);        // 左前方向的形心，单位为弧度（-90°）
	FuzzyCentroidStruct.Code_Beta_Front = 0;                         // 正前方向的形心，单位为弧度（0°）
	FuzzyCentroidStruct.Code_Beta_Right_Front = 0.25f * PI;          // 右前方向的形心，单位为弧度（45°）
	FuzzyCentroidStruct.Code_Beta_Right = 0.5f * PI;                 // 正右方向的形心，单位为弧度（90°）
	
	beta_core[0] = FuzzyCentroidStruct.Code_Beta_Left;
	beta_core[1] = FuzzyCentroidStruct.Code_Beta_Left_Front;
	beta_core[2] = FuzzyCentroidStruct.Code_Beta_Front;
	beta_core[3] = FuzzyCentroidStruct.Code_Beta_Right_Front;
	beta_core[4] = FuzzyCentroidStruct.Code_Beta_Right;
	
	speed_core[0] = FuzzyCentroidStruct.Code_Speed_Fast;
	speed_core[1] = FuzzyCentroidStruct.Code_Speed_Slow;	
}

/**
@brief      根据左边，前方，右边的障碍物的距离信息进行模糊决策
@param      dl 左边障碍物的距离，单位为m
            df 前方障碍物的距离，单位为m
            dr 右边障碍物的距离，单位为m
@retval     模糊决策的结果，包含两电机的速度，单位为dm/s
*/
FUZZY_ST fuzzy_decision(float dl, float df, float dr)
{
    fuzzifier(&dl);
    return defuzzifier();
}
/**
@brief      对三个方向障碍物的距离信息进行模糊化
@param      fp 通过fp指针可以访问到传入fuzzy_decision函数的三个参数，避免传递过多参数
@retval     None
@note       这是一个私有函数；注释掉的部分可以结合上位机通过串口进行调试，寻找优化的参数
*/
static void fuzzifier(const float *fp)
{
    float tmp = 0.0f;

    for (int i = 0; i < 3; ++i)
    {
        if (*fp <= DISTANCE_NEAR)
        {
            distance[i][1] = 1;
        }
        else if (*fp >= DISTANCE_FAR)
        {
            distance[i][1] = 0;
        }
        else
        {
            distance[i][1] = (DISTANCE_FAR - *fp) / (DISTANCE_FAR - DISTANCE_NEAR);
        }
        //      if(*fp<=threshold_barrier_near){
        //          distance[i][1]=1;
        //      }else if (*fp>=threshold_barrier_far){
        //          distance[i][1]=0;
        //      }else{
        //          distance[i][1]=(threshold_barrier_far-*fp)/(threshold_barrier_far-threshold_barrier_near);
        //      }
        distance[i][0] = 1 - distance[i][1];

        fp++;
    }
		

    v[0] = beta[2] = MIN_3(distance[0][0], distance[1][0], distance[2][0]); //O1,v快,正前
    v[1] = beta[4] = MIN_3(distance[0][0], distance[1][1], distance[2][0]); //O2,v慢,正右
    beta[1] = MIN_3(distance[0][0], distance[1][0], distance[2][1]);        //O3,左前
    v[1] = MAX_2(v[1], beta[1]);                                            //v慢
    beta[0] = MIN_3(distance[0][0], distance[1][1], distance[2][1]);        //O4,正左
    v[1] = MAX_2(v[1], beta[0]);                                            //v慢
    beta[3] = MIN_3(distance[0][1], distance[1][0], distance[2][0]);        //O5,右前
    v[1] = MAX_2(v[1], beta[3]);                                            //v慢
    tmp = MIN_3(distance[0][1], distance[1][1], distance[2][0]);            //O6
    v[1] = MAX_2(v[1], tmp);                                                //v慢
    beta[4] = MAX_2(beta[4], tmp);                                          //正右
    tmp = MIN_3(distance[0][1], distance[1][0], distance[2][1]);            //O7
    v[1] = MAX_2(v[1], tmp);                                                //v慢
    beta[2] = MAX_2(beta[2], tmp);                                          //正前
    tmp = MIN_3(distance[0][1], distance[1][1], distance[2][1]);            //O8
    v[1] = MAX_2(v[1], tmp);                                                //v慢
    beta[4] = MAX_2(beta[4], tmp);                                          //正右
		
//		printf("v1=%.2f,v2=%.2f\r\n",v[0],v[1]);
//		printf("beta1=%.2f,beta2=%.2f,beta3=%.2f,beta4=%.2f,beta5=%.2f\r\n",beta[0],beta[1],beta[2],beta[3],beta[4]);
}
/**
@brief      将模糊化后得到的信息反模糊化，获得两个电机的速度
@param      None
@retval     模糊决策的结果，包含两电机的速度，单位为dm/s
*/
FUZZY_ST defuzzifier(void)
{
	FUZZY_ST two_speed;     ///<存储模糊决策的结果
	float defuzzy_v = 0.0f;    ///<反模糊化得到的小车整体速度
	float defuzzy_beta = 0.0f; ///<反模糊化得到的小车应该偏移的角度
	float sum = 0.0f;

	//  speed_core[0]=speed_fast;
	//  speed_core[1]=speed_low;

	for (int i = 0; i < SPEED_SUM; ++i)
	{
			defuzzy_v += v[i] * speed_core[i];
			sum += v[i];
	}
	defuzzy_v /= sum;

	sum = 0;
	for (int i = 0; i < BETA_SUM; ++i)
	{
			defuzzy_beta += beta[i] * beta_core[i];
			sum += beta[i];
	}
	defuzzy_beta /= sum;
		
	two_speed.turnAngle = (defuzzy_beta/PI*180);
	
	if(defuzzy_beta/PI*180.0f < 0)
	{
		two_speed.turnDir = 4;   //左转
	}
	else 
	{
		two_speed.turnDir = 2;   //右转
	}
	
//	printf("currentFWValue = %.2f,adjustedFWValue = %.2f\r\n",currentFWValue, adjustedFWValue);

	two_speed.speed0 = two_speed.speed1 = defuzzy_v;
	
	return two_speed;
}



