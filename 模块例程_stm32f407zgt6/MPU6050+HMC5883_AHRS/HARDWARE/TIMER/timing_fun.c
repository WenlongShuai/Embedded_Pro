#include "timing_fun.h"

#include "MPU6050.h"
#include "HMC5883L.h"
#include "sensor_calibration.h"
#include "MadgwickAHRS.h"
#include <math.h>

// 全局变量
int16_t acc_raw[3], gyro_raw[3], mag_raw[3];
int16_t acc_cal[3], gyro_cal[3], mag_cal[3];
int16_t temperature = 0;
float acc_cal_temp[3], gyro_cal_temp[3], mag_cal_temp[3];

// 初始化卡尔曼滤波器
KalmanFilter kf;
float sensor_data[SENSOR_DIM];
// 时间步长
float dt = 0.01f;  // 100Hz采样率

// 计算向量范数（3D）
static float VectorNorm(float x, float y, float z) {
    return sqrtf(x * x + y * y + z * z);
}

// 计算向量范数（4D，用于四元数）
static float VectorNorm4(float x, float y, float z, float w) {
    return sqrtf(x * x + y * y + z * z + w * w);
}

// 向量归一化
static void NormalizeVector(float *x, float *y, float *z) {
    float norm = VectorNorm(*x, *y, *z);
    if (norm > 0.0f) {
        *x /= norm;
        *y /= norm;
        *z /= norm;
    }
}

/**************************************************************************
函数功能：定时中断初始化
入口参数：arr：自动重装值  psc：时钟预分频数 
返回  值：无
**************************************************************************/
void Timing_Fun_Init(u16 arr,u16 psc)  
{  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr;               //重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;            //预分频系数
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;           //时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);               //使能定时器中断
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;        //使能按键所在的外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //使能外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;        //响应优先级3
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);
}  


/**************************************************************************
函数功能：所有的控制代码都在这里面
          TIM1控制的定时中断 
**************************************************************************/
void TIM2_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET)//定时中断
	{   
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                             //===清除定时器5中断标志位
		
		#if 1
		READ_HMC5883L(&mag_raw[0], &mag_raw[1], &mag_raw[2]);
		mpu6050_get_temperature(&temperature);
		mpu6050_get_gyroscope(&gyro_raw[0], &gyro_raw[1], &gyro_raw[2]);
		mpu6050_get_accelerometer(&acc_raw[0], &acc_raw[1], &acc_raw[2]);
		
		// 应用校准参数
		SensorCalibration_Apply(acc_raw, gyro_raw, mag_raw, 
														 acc_cal, gyro_cal, mag_cal);
		
		sensor_data[0] = acc_cal[0] / 1.0;
		sensor_data[1] = acc_cal[1] / 1.0;
		sensor_data[2] = acc_cal[2] / 1.0;
		sensor_data[3] = gyro_cal[0] / 1.0;
		sensor_data[4] = gyro_cal[1] / 1.0;
		sensor_data[5] = gyro_cal[2] / 1.0;
		sensor_data[6] = mag_cal[0] / 1.0;
		sensor_data[7] = mag_cal[1] / 1.0;
		sensor_data[8] = mag_cal[2] / 1.0;			
		
		
		// 执行一次滤波迭代
		KalmanFilter_Predict(&kf, dt);
		KalmanFilter_Update(&kf, sensor_data);

		// 获取滤波后的数据
		float filtered_data[SENSOR_DIM];
		KalmanFilter_GetFilteredData(&kf, filtered_data);
		
		acc_cal_temp[0] = 0.000061035f * acc_cal[0];
		acc_cal_temp[1] = 0.000061035f * acc_cal[1];
		acc_cal_temp[2] = 0.000061035f * acc_cal[2];				
		
		gyro_cal_temp[0] = 0.061035 * gyro_cal[0];
		gyro_cal_temp[1] = 0.061035 * gyro_cal[1];
		gyro_cal_temp[2] = 0.061035 * gyro_cal[2];				
		
		mag_cal_temp[0] = 0.635 * mag_cal[0];
		mag_cal_temp[1] = 0.635 * mag_cal[1];
		mag_cal_temp[2] = 0.635 * mag_cal[2];
		
		
		 // 归一化加速度计和磁力计数据
		NormalizeVector(&acc_cal_temp[0], &acc_cal_temp[1], &acc_cal_temp[2]);
		NormalizeVector(&mag_cal_temp[0], &mag_cal_temp[1], &mag_cal_temp[2]);

		MadgwickAHRSupdate(
		gyro_cal_temp[0]*DEG2RAD, gyro_cal_temp[1]*DEG2RAD, gyro_cal_temp[2]*DEG2RAD, 
		acc_cal_temp[0], acc_cal_temp[1], acc_cal_temp[2],
		mag_cal_temp[1], mag_cal_temp[0], mag_cal_temp[2]);//磁力计坐标系与IMU坐标系不一致，xy互换
		
		computeAngles();
		
		printf("roll: %.2f, pitch: %.2f, yaw: %.2f\r\n", roll, pitch, yaw);
		
		#endif
	}
}

