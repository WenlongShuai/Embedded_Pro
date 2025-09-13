#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "stm32f4xx_conf.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

// 超声波数据结构体
typedef struct 
{
	uint8_t sersor_ID;    // 超声波传感器ID
	uint32_t start_time;  // 上升沿捕获时间（us）
  uint32_t end_time;    // 下降沿捕获时间（us）
  uint32_t time_diff;   // 时间差（us）
  uint8_t flag;         // 状态标志：0=未捕获上升沿，1=已捕获上升沿
  uint8_t overflow_cnt; // 定时器溢出次数（处理长脉冲）
	EventGroupHandle_t event_group;  //事件组
	TimerHandle_t timeout_timer;   //超时定时器
	QueueHandle_t data_queue;      //队列
} ultrasonic_sensor_t;

// 超声波有效\无效值结构体
typedef struct {
    float distance_cm;  // 距离值（厘米）
    uint8_t is_valid;          // 数据有效性标志（true=有效，false=超时）
    uint8_t sensor_id;      // 传感器标识（0～N-1）
    // 可扩展字段（如时间戳、信号强度等）
    // uint32_t timestamp;
    // uint8_t signal_strength;
} ultrasonic_data_t;


// 事件标志位
#define ULTRASONIC_DATA_READY_BIT (1 << 0)   // 超声波数据有效事件
#define ULTRASONIC_TIMEOUT_BIT    (1 << 1)   // 超声波数据超时事件
 
void Hcsr04Init(void);
void Hcsr04Start(uint8_t num);

#endif
