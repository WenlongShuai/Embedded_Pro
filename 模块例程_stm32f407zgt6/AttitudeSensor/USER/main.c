#include <stdio.h>
#include "usart.h"
#include "systick.h"
#include "MPU_HMC_ADXL.h"
#include "MadgwickAHRS.h"
#include "DMA.h"

float gx = 0;
float gy = 0;
float gz = 0;

float ax = 0;
float ay = 0;
float az = 0;

float mx = 0;
float my = 0;
float mz = 0;
double angle = 0;

float MagSensAdj[3] = {1, 0.9382, 0.904};

void quaternionConvertEulerAngle(float *Yaw, float *Pitch, float *Roll);
float angleNormalization(double angle);

int main()
{
//	float yaw = 0;
//	float pitch = 0;
//	float roll = 0;
	
	SysTick_Init();
	uart_init(115200);
	
	MPU_HMC_ADXL_Init();
	
	CalibrateHMC5883();
	
	while(1)
	{
		//读取原始数据  I2C
		ReadData_MPU3050(&gx, &gy, &gz);
		//printf("gx：%f\tgy：%f\tgz：%f\r\n",gx, gy, gz);
		ReadData_ADXL345(&ax, &ay, &az);
		//printf("ax：%f ay：%f az：%f\r\n",ax, ay, az);
		ReadData_HMC5883(&mx, &my, &mz);
		//printf("mx：%f my：%f mz：%f\r\n",mx, my, mz);
		
		MadgwickAHRSupdate(
				(float)gx/GYRO_SENSI*DEG2RAD, (float)gy/GYRO_SENSI*DEG2RAD, (float)gz/GYRO_SENSI*DEG2RAD, 
				(float)ax/ACC_SENSI, (float)ay/ACC_SENSI, (float)az/ACC_SENSI,
				(float)my*MagSensAdj[1]/MAG_SENSI,(float)mx*MagSensAdj[0]/MAG_SENSI,(float)mz*MagSensAdj[2]/MAG_SENSI);//磁力计坐标系与IMU坐标系不一致，xy互换
		
				
		computeAngles();
				
		printf("航向：%.2f° 俯仰：%.2f° 横滚：%.2f°\r\n",yaw, pitch, roll);
				
//		//读取原始数据  I2C
//		ReadData_MPU3050(&gx, &gy, &gz);
//		//printf("gx：%f\tgy：%f\tgz：%f\r\n",gx, gy, gz);
//		ReadData_ADXL345(&ax, &ay, &az);
//		//printf("ax：%f ay：%f az：%f\r\n",ax, ay, az);
//		ReadData_HMC5883(&mx, &my, &mz);
//		//printf("mx：%f my：%f mz：%f\r\n",mx, my, mz);
//		
//		angle = AngleToDirection(mx, my, mz);
//		
//		quaternionConvertEulerAngle(&yaw, &pitch, &roll);
//		//printf("航向：%.2f° 俯仰：%.2f° 横滚：%.2f°方位：%.2f\r\n",yaw, pitch, roll, angle);
//		Delay_ms(100);
		
//		//读取原始数据 DMA
//		ReadData_MPU3050_DMA(&gx, &gy, &gz);
//		//printf("%f\t %f\t %f\r\n",gx, gy, gz);
//		ReadData_ADXL345_DMA(&ax, &ay, &az);
//		//printf("%f\t %f\t %f\r\n",ax, ay, az);
//		ReadData_HMC5883_DMA(&mx, &my, &mz);
//		//printf("%f\t %f\t %f\r\n",mx, my, mz);
//		
//		angle = AngleToDirection(mx, my, mz);
//		
//		quaternionConvertEulerAngle(&yaw, &pitch, &roll);
//		printf("航向：%.2f° 俯仰：%.2f° 横滚：%.2f°方位：%.2f\r\n",yaw, pitch, roll, angle);
//		//printf("%.2f\t %.2f\t %.2f\t %.2f\r\n",yaw, pitch, roll, angle);

//		Delay_ms(100);
	}
	
	return 0;
}


//四元数转欧拉角
void quaternionConvertEulerAngle(float *Yaw, float *Pitch, float *Roll)
{
	#if 0
	//将原始数据转换四元数
	
	MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
	
	
//	printf("q0 ---> %f\r\n",q0);
//	printf("q1 ---> %f\r\n",q1);
//	printf("q2 ---> %f\r\n",q2);
//	printf("q3 ---> %f\r\n",q3);
	
	double sin_theta = 2*(q0*q2-q3*q1);
	if(fabs(sin_theta) >= 1)
	{
		*Pitch = (copysign(M_PI/2,sin_theta))* (180.0 / M_PI);  //弧度*(180.0 / M_PI)
	}
	else
	{
		*Pitch = asin(sin_theta)* (180.0 / M_PI);//弧度*(180.0 / M_PI)
	}
	
	double cos_theta = cos(*Pitch);
	
	if(cos_theta != 0)
	{
		*Roll = atan2(2*(q0*q3 + q1*q2), 1 - 2*(pow(q2,2) + pow(q3,2)))* (180.0 / M_PI);//弧度*(180.0 / M_PI)
		*Yaw = angleNormalization(atan2(2*(q0*q1 + q2*q3), 1 - 2*(pow(q1,2) + pow(q2,2)))* (180.0 / M_PI)); //弧度*(180.0 / M_PI)
	}
	else
	{
		*Roll = atan2(2*(q0*q1 + q2*q3), 2*(pow(q0,2) + pow(q1,2))-1)* (180.0 / M_PI);//弧度*(180.0 / M_PI)
		*Yaw = angleNormalization(0* (180.0 / M_PI)); //弧度*(180.0 / M_PI)
	}
#endif

#if 0
	// 初始化四元数
	Quaternion q = {1, 0, 0, 0};

	// 示例传感器数据
	double gyro[3] = {gx, gy, gz};
	double accel[3] = {ax, ay, az};
	double mag[3] = {mx, my, mz};
	double dt = 0.001;
	
	Quaternion q_updated = ekfU pdate(q, gyro, accel, mag, dt);
	q0 = q_updated.w;
	q1 = q_updated.x;
	q2 = q_updated.y;
	q3 = q_updated.z;
	
	double sin_theta = 2*(q0*q2-q3*q1);
	if(fabs(sin_theta) >= 1)
	{
		*Pitch = (copysign(M_PI/2,sin_theta))* (180.0 / M_PI);  //弧度*(180.0 / M_PI)
	}
	else
	{
		*Pitch = asin(sin_theta)* (180.0 / M_PI);//弧度*(180.0 / M_PI)
	}

	double cos_theta = cos(*Pitch);

	if(cos_theta != 0)
	{
		*Roll = atan2(2*(q0*q3 + q1*q2), 1 - 2*(pow(q2,2) + pow(q3,2)))* (180.0 / M_PI);//弧度*(180.0 / M_PI)
		*Yaw = angleNormalization(atan2(2*(q0*q1 + q2*q3), 1 - 2*(pow(q1,2) + pow(q2,2)))* (180.0 / M_PI)); //弧度*(180.0 / M_PI)
	}
	else
	{
		*Roll = atan2(2*(q0*q1 + q2*q3), 2*(pow(q0,2) + pow(q1,2))-1)* (180.0 / M_PI);//弧度*(180.0 / M_PI)
		*Yaw = angleNormalization(0* (180.0 / M_PI)); //弧度*(180.0 / M_PI)
	}
	#endif
}

float angleNormalization(double angle)
{
	if(angle >= 360)
	{
		angle -= 360;
	}
	else if(angle < 0)
	{
		angle += 360;
	}
	
	return angle;
}

