#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "stm32f4xx_conf.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

// ���������ݽṹ��
typedef struct 
{
	uint8_t sersor_ID;    // ������������ID
	uint32_t start_time;  // �����ز���ʱ�䣨us��
  uint32_t end_time;    // �½��ز���ʱ�䣨us��
  uint32_t time_diff;   // ʱ��us��
  uint8_t flag;         // ״̬��־��0=δ���������أ�1=�Ѳ���������
  uint8_t overflow_cnt; // ��ʱ������������������壩
	EventGroupHandle_t event_group;  //�¼���
	TimerHandle_t timeout_timer;   //��ʱ��ʱ��
	QueueHandle_t data_queue;      //����
} ultrasonic_sensor_t;

// ��������Ч\��Чֵ�ṹ��
typedef struct {
    float distance_cm;  // ����ֵ�����ף�
    uint8_t is_valid;          // ������Ч�Ա�־��true=��Ч��false=��ʱ��
    uint8_t sensor_id;      // ��������ʶ��0��N-1��
    // ����չ�ֶΣ���ʱ������ź�ǿ�ȵȣ�
    // uint32_t timestamp;
    // uint8_t signal_strength;
} ultrasonic_data_t;


// �¼���־λ
#define ULTRASONIC_DATA_READY_BIT (1 << 0)   // ������������Ч�¼�
#define ULTRASONIC_TIMEOUT_BIT    (1 << 1)   // ���������ݳ�ʱ�¼�
 
void Hcsr04Init(void);
void Hcsr04Start(uint8_t num);

#endif
