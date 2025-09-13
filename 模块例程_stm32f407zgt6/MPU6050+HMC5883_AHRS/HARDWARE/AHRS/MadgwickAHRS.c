//=====================================================================================================
// MadgwickAHRS.c
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "MadgwickAHRS.h"
#include <math.h>

//---------------------------------------------------------------------------------------------------
// Definitions

#define sampleFreq	100.0f		// sample frequency in Hz
#define betaDef		0.1f		// 2 * proportional gain


//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float beta = betaDef;								// 2 * proportional gain (Kp)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame
volatile float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;

volatile int tst_q0, tst_q1, tst_q2, tst_q3;
volatile int tst_yaw, tst_roll, tst_pitch;
//---------------------------------------------------------------------------------------------------
// Function declarations

float invSqrt(float x);

//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az);
		return;
	}

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;   

		// Normalise magnetometer measurement
		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0f * q0 * mx;
		_2q0my = 2.0f * q0 * my;
		_2q0mz = 2.0f * q0 * mz;
		_2q1mx = 2.0f * q1 * mx;
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_2q0q2 = 2.0f * q0 * q2;
		_2q2q3 = 2.0f * q2 * q3;
		q0q0 = q0 * q0;
		q0q1 = q0 * q1;
		q0q2 = q0 * q2;
		q0q3 = q0 * q3;
		q1q1 = q1 * q1;
		q1q2 = q1 * q2;
		q1q3 = q1 * q3;
		q2q2 = q2 * q2;
		q2q3 = q2 * q3;
		q3q3 = q3 * q3;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		// Gradient decent algorithm corrective step
		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

//---------------------------------------------------------------------------------------------------
// IMU algorithm update

void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;   

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

//---------------------------------------------------------------------------------------------------
//
void computeAngles(void)
{
	roll = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2) * 57.3f;
	pitch = asinf(-2.0f * (q1*q3 - q0*q2))* 57.3f;
	yaw = atan2f(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3)* 57.3f;
	if(yaw < 0)
		yaw = yaw + 360;
	
	tst_q0 = q0 * 100;
	tst_q1 = q1 * 100;
	tst_q2 = q2 * 100;
	tst_q3 = q3 * 100;
	
	tst_pitch = pitch * 100;
	tst_roll = roll*100;
	tst_yaw = yaw*100;
	
}


// 矩阵运算辅助函数
void matrix_multiply(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                     float C[SENSOR_DIM][SENSOR_DIM]) {
    float temp[SENSOR_DIM][SENSOR_DIM] = {0};
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            for (int k = 0; k < SENSOR_DIM; k++) {
                temp[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            C[i][j] = temp[i][j];
        }
    }
}
										 
void matrix_add(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                float C[SENSOR_DIM][SENSOR_DIM]) {
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

void matrix_subtract(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM], 
                     float C[SENSOR_DIM][SENSOR_DIM]) {
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
}

// 矩阵求逆（简化版，仅处理对角矩阵）
void matrix_inverse(float A[SENSOR_DIM][SENSOR_DIM], float B[SENSOR_DIM][SENSOR_DIM]) {
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            B[i][j] = (i == j) ? 1.0f / A[i][i] : 0.0f;
        }
    }
}

// 初始化卡尔曼滤波器
void KalmanFilter_Init(KalmanFilter *kf) {
    // 初始化状态向量（全零）
    for (int i = 0; i < SENSOR_DIM; i++) {
        kf->x[i] = 0.0f;
    }
    
    // 初始化协方差矩阵
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            kf->P[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    // 初始化过程噪声协方差（根据传感器特性调整）
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            kf->Q[i][j] = (i == j) ? 0.001f : 0.0f;
        }
    }
    // 陀螺仪通常噪声更大
    kf->Q[3][3] = 0.002f;
    kf->Q[4][4] = 0.002f;
    kf->Q[5][5] = 0.002f;
    
    // 初始化测量噪声协方差（根据传感器特性调整）
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            kf->R[i][j] = (i == j) ? 0.1f : 0.0f;
        }
    }
    // 磁力计通常噪声更大
    kf->R[6][6] = 0.5f;
    kf->R[7][7] = 0.5f;
    kf->R[8][8] = 0.5f;
}

// 预测步骤（使用上一状态预测当前状态）
void KalmanFilter_Predict(KalmanFilter *kf, float dt) {
    // 状态预测（假设传感器数据缓慢变化）
    // x = F*x + B*u (这里简化为 x = x)
    
    // 更新协方差矩阵
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            kf->P[i][j] += kf->Q[i][j] * dt;
        }
    }
}

// 更新步骤（融合新的测量值）
void KalmanFilter_Update(KalmanFilter *kf, float z[SENSOR_DIM]) {
    float K[SENSOR_DIM][SENSOR_DIM];  // 卡尔曼增益
    float PHT[SENSOR_DIM][SENSOR_DIM]; // P * H^T
    float S[SENSOR_DIM][SENSOR_DIM];   // 测量协方差
    float SI[SENSOR_DIM][SENSOR_DIM];  // S的逆
    
    // 计算卡尔曼增益
    // H 是单位矩阵（测量直接对应状态）
    // K = P * H^T * (H * P * H^T + R)^-1
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            PHT[i][j] = kf->P[i][j];  // H^T 是单位矩阵
            S[i][j] = kf->P[i][j] + kf->R[i][j];
        }
    }
    
    // 计算S的逆（简化版，假设S是对角矩阵）
    matrix_inverse(S, SI);
    
    // 计算卡尔曼增益
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            K[i][j] = 0.0f;
            for (int k = 0; k < SENSOR_DIM; k++) {
                K[i][j] += PHT[i][k] * SI[k][j];
            }
        }
    }
    
    // 更新状态估计
    float y[SENSOR_DIM];  // 测量残差
    for (int i = 0; i < SENSOR_DIM; i++) {
        y[i] = z[i] - kf->x[i];  // z = H*x + v (H是单位矩阵)
        kf->x[i] += K[i][i] * y[i];  // 简化：只使用对角元素
    }
    
    // 更新协方差矩阵
    float I_KH[SENSOR_DIM][SENSOR_DIM];  // I - K*H
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            I_KH[i][j] = (i == j) ? (1.0f - K[i][j]) : -K[i][j];
        }
    }
    
    float temp[SENSOR_DIM][SENSOR_DIM];
    matrix_multiply(I_KH, kf->P, temp);
    for (int i = 0; i < SENSOR_DIM; i++) {
        for (int j = 0; j < SENSOR_DIM; j++) {
            kf->P[i][j] = temp[i][j];
        }
    }
}


// 获取滤波后的数据
void KalmanFilter_GetFilteredData(KalmanFilter *kf, float filtered_data[SENSOR_DIM]) {
    for (int i = 0; i < SENSOR_DIM; i++) {
        filtered_data[i] = kf->x[i];
    }
}

//====================================================================================================
// END OF CODE
//====================================================================================================
