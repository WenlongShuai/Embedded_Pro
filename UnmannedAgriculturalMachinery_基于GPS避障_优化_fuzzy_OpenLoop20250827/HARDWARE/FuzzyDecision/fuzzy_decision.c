/**
@file       fuzzy_decision.c
@brief      ʵ����ģ���߼��йصĺ��� 
*/

#include "fuzzy_decision.h"

#include "fun.h"

extern float RC_Velocity;
extern float adjustedFWValue;
extern float currentFWValue;

static float distance[3][2];                                                                                                        // ��һά���α�ʾ����ϰ���ľ���,ǰ���ϰ���ľ���,�ұ��ϰ���ľ��룬�ڶ�ά���α�ʾԶ����
static float v[SPEED_SUM];                                                                                                          // �ܹ�����λ�����α�ʾ�ٶȿ죬�ٶ���
static float beta[BETA_SUM];                                                                                                        // ���α�ʾ������ǰ����ǰ����ǰ������
static float beta_core[BETA_SUM] = {0.0f}; // Ϊ�˷�����
float speed_core[SPEED_SUM] = {0.0f};  		 // Ϊ�˷�����

static void fuzzifier(const float *fp);
static FUZZY_ST defuzzifier(void);
static Fuzzy_Centroid FuzzyCentroidStruct;

void fuzzy_Init(void)
{
	FuzzyCentroidStruct.Code_Speed_Fast = 0;                         // �ٶȿ�����ģ���λΪdm/s
	FuzzyCentroidStruct.Code_Speed_Slow = 0;                         // �ٶ��������ģ���λΪdm/s
	FuzzyCentroidStruct.Code_Beta_Left = -(0.5f * PI);               // ����������ģ���λΪ���ȣ�-45�㣩
	FuzzyCentroidStruct.Code_Beta_Left_Front = -(0.25f * PI);        // ��ǰ��������ģ���λΪ���ȣ�-90�㣩
	FuzzyCentroidStruct.Code_Beta_Front = 0;                         // ��ǰ��������ģ���λΪ���ȣ�0�㣩
	FuzzyCentroidStruct.Code_Beta_Right_Front = 0.25f * PI;          // ��ǰ��������ģ���λΪ���ȣ�45�㣩
	FuzzyCentroidStruct.Code_Beta_Right = 0.5f * PI;                 // ���ҷ�������ģ���λΪ���ȣ�90�㣩
	
	beta_core[0] = FuzzyCentroidStruct.Code_Beta_Left;
	beta_core[1] = FuzzyCentroidStruct.Code_Beta_Left_Front;
	beta_core[2] = FuzzyCentroidStruct.Code_Beta_Front;
	beta_core[3] = FuzzyCentroidStruct.Code_Beta_Right_Front;
	beta_core[4] = FuzzyCentroidStruct.Code_Beta_Right;
	
	speed_core[0] = FuzzyCentroidStruct.Code_Speed_Fast;
	speed_core[1] = FuzzyCentroidStruct.Code_Speed_Slow;	
}

/**
@brief      ������ߣ�ǰ�����ұߵ��ϰ���ľ�����Ϣ����ģ������
@param      dl ����ϰ���ľ��룬��λΪm
            df ǰ���ϰ���ľ��룬��λΪm
            dr �ұ��ϰ���ľ��룬��λΪm
@retval     ģ�����ߵĽ����������������ٶȣ���λΪdm/s
*/
FUZZY_ST fuzzy_decision(float dl, float df, float dr)
{
    fuzzifier(&dl);
    return defuzzifier();
}
/**
@brief      �����������ϰ���ľ�����Ϣ����ģ����
@param      fp ͨ��fpָ����Է��ʵ�����fuzzy_decision�������������������⴫�ݹ������
@retval     None
@note       ����һ��˽�к�����ע�͵��Ĳ��ֿ��Խ����λ��ͨ�����ڽ��е��ԣ�Ѱ���Ż��Ĳ���
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
		

    v[0] = beta[2] = MIN_3(distance[0][0], distance[1][0], distance[2][0]); //O1,v��,��ǰ
    v[1] = beta[4] = MIN_3(distance[0][0], distance[1][1], distance[2][0]); //O2,v��,����
    beta[1] = MIN_3(distance[0][0], distance[1][0], distance[2][1]);        //O3,��ǰ
    v[1] = MAX_2(v[1], beta[1]);                                            //v��
    beta[0] = MIN_3(distance[0][0], distance[1][1], distance[2][1]);        //O4,����
    v[1] = MAX_2(v[1], beta[0]);                                            //v��
    beta[3] = MIN_3(distance[0][1], distance[1][0], distance[2][0]);        //O5,��ǰ
    v[1] = MAX_2(v[1], beta[3]);                                            //v��
    tmp = MIN_3(distance[0][1], distance[1][1], distance[2][0]);            //O6
    v[1] = MAX_2(v[1], tmp);                                                //v��
    beta[4] = MAX_2(beta[4], tmp);                                          //����
    tmp = MIN_3(distance[0][1], distance[1][0], distance[2][1]);            //O7
    v[1] = MAX_2(v[1], tmp);                                                //v��
    beta[2] = MAX_2(beta[2], tmp);                                          //��ǰ
    tmp = MIN_3(distance[0][1], distance[1][1], distance[2][1]);            //O8
    v[1] = MAX_2(v[1], tmp);                                                //v��
    beta[4] = MAX_2(beta[4], tmp);                                          //����
		
//		printf("v1=%.2f,v2=%.2f\r\n",v[0],v[1]);
//		printf("beta1=%.2f,beta2=%.2f,beta3=%.2f,beta4=%.2f,beta5=%.2f\r\n",beta[0],beta[1],beta[2],beta[3],beta[4]);
}
/**
@brief      ��ģ������õ�����Ϣ��ģ�������������������ٶ�
@param      None
@retval     ģ�����ߵĽ����������������ٶȣ���λΪdm/s
*/
FUZZY_ST defuzzifier(void)
{
	FUZZY_ST two_speed;     ///<�洢ģ�����ߵĽ��
	float defuzzy_v = 0.0f;    ///<��ģ�����õ���С�������ٶ�
	float defuzzy_beta = 0.0f; ///<��ģ�����õ���С��Ӧ��ƫ�ƵĽǶ�
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
		two_speed.turnDir = 4;   //��ת
	}
	else 
	{
		two_speed.turnDir = 2;   //��ת
	}
	
//	printf("currentFWValue = %.2f,adjustedFWValue = %.2f\r\n",currentFWValue, adjustedFWValue);

	two_speed.speed0 = two_speed.speed1 = defuzzy_v;
	
	return two_speed;
}



