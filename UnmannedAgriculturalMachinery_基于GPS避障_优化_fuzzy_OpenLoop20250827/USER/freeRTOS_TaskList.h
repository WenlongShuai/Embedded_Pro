#ifndef __FREERTOS_TASKLIST_H
#define __FREERTOS_TASKLIST_H

#include "sys.h"

#define TASK_NUM 		 12    // 任务数量

#define CARTYPE      1   // 车子类型      0：小车    1：大车
  
#define WORKMODE           1    // 工作模式，0：采集模式    1：其他

#if !WORKMODE   // 采集工作模式
#define COLLECT_GPS_MODE         1    //采集GPS数据的方式　　　0：自动采集　1：手动采集　
#else
// 避障模式， 0：模糊避障（模糊决策算法计算出避障角度，通过一系列不同的角度避开障碍物）    1：简单避障（遇到障碍物停止，障碍物离开启动）
#define OBSTACLE_AVOIDANCE_MODE   1
#define GETDISMODE   0   // 距离获取方式，0：超声波   1：激光
#define DISMODE      0   // 显示方式      0：OLED    1：LCD
//避障阈值
#define LEFT_THRESHOLDVALUE		180
#define MID_THRESHOLDVALUE		180
#define RIGHT_THRESHOLDVALUE	  180
#define LEFTSIDE_THRESHOLDVALUE		100
#define RIGHTSIDE_THRESHOLDVALUE	100


#endif

// 测距传感器数量
#define SENSORNUM 	5

typedef void (*taskPtr)(void);

void freertos_setStartTask(void);


#endif /* __FREERTOS_TASKLIST_H */
