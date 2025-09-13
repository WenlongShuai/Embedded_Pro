#ifndef __FREERTOS_TASKLIST_H
#define __FREERTOS_TASKLIST_H

#include "sys.h"

#define TASK_NUM 		 12    // ��������

#define CARTYPE      1   // ��������      0��С��    1����
  
#define WORKMODE           1    // ����ģʽ��0���ɼ�ģʽ    1������

#if !WORKMODE   // �ɼ�����ģʽ
#define COLLECT_GPS_MODE         1    //�ɼ�GPS���ݵķ�ʽ������0���Զ��ɼ���1���ֶ��ɼ���
#else
// ����ģʽ�� 0��ģ�����ϣ�ģ�������㷨��������ϽǶȣ�ͨ��һϵ�в�ͬ�ĽǶȱܿ��ϰ��    1���򵥱��ϣ������ϰ���ֹͣ���ϰ����뿪������
#define OBSTACLE_AVOIDANCE_MODE   1
#define GETDISMODE   0   // �����ȡ��ʽ��0��������   1������
#define DISMODE      0   // ��ʾ��ʽ      0��OLED    1��LCD
//������ֵ
#define LEFT_THRESHOLDVALUE		180
#define MID_THRESHOLDVALUE		180
#define RIGHT_THRESHOLDVALUE	  180
#define LEFTSIDE_THRESHOLDVALUE		100
#define RIGHTSIDE_THRESHOLDVALUE	100


#endif

// ��ഫ��������
#define SENSORNUM 	5

typedef void (*taskPtr)(void);

void freertos_setStartTask(void);


#endif /* __FREERTOS_TASKLIST_H */
