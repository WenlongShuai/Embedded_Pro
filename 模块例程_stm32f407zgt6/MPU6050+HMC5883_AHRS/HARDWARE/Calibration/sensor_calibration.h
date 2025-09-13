#ifndef __SENSOR_CALIBRATION_H
#define __SENSOR_CALIBRATION_H

#include "sys.h"


#define ACC_SAMPLE_COUNT  100  // 加速度计采样点数
#define GYRO_SAMPLE_COUNT 100  // 陀螺仪采样点数
#define MAG_SAMPLE_COUNT  500  // 磁力计采样点数

// 传感器校准参数结构体
typedef struct {
    // 加速度计校准参数
    float acc_bias[3];       // 零偏
    float acc_scale[3];      // 比例因子
    
    // 陀螺仪校准参数
    float gyro_bias[3];      // 零偏
		
    
    // 磁力计校准参数
    float mag_bias[3];       // 硬磁偏移
//    float mag_scale[3][3];   // 软磁变换矩阵
	float mag_scale[3];
} SensorCalibData;

// 校准状态枚举
typedef enum {
    CALIB_IDLE,             // 空闲状态
    CALIB_ACC_COLLECTING,   // 加速度计数据采集
    CALIB_GYRO_COLLECTING,  // 陀螺仪数据采集
    CALIB_MAG_COLLECTING,   // 磁力计数据采集
    CALIB_MAG_PROCESSING,   // 磁力计数据处理
    CALIB_COMPLETED         // 校准完成
} CalibrationState;

// 函数声明
void SensorCalibration_Init(void);
void SensorCalibration_Process(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3]);
CalibrationState SensorCalibration_GetState(void);
void SensorCalibration_GetData(SensorCalibData *calib_data);
void SensorCalibration_Apply(int16_t acc_raw[3], int16_t gyro_raw[3], int16_t mag_raw[3], 
                             int16_t acc_cal[3], int16_t gyro_cal[3], int16_t mag_cal[3]);
                             
void SensorCalibration_StartAccCalibration(void);
void SensorCalibration_StartGyroCalibration(void);
void SensorCalibration_StartMagCalibration(void);

#endif   /* __SENSOR_CALIBRATION_H */
