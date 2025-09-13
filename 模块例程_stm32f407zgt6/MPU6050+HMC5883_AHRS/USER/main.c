#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timing_fun.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "sensor_calibration.h"
#include "MadgwickAHRS.h"

extern float  angle;
extern KalmanFilter kf;

static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed);
static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z);
static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len);
void Usart_SendString(uint8_t *str, uint32_t len);

void NormalizeVector(float *x, float *y, float *z);

extern volatile float pitch, roll, yaw;


//int16_t acc_raw[3], gyro_raw[3], mag_raw[3];
//int16_t acc_cal[3], gyro_cal[3], mag_cal[3];

int main(void)
{	
	uint8_t ret = 0;
	
//	uart_init(500000);
	uart_init(115200);
	delay_init();                                   //初始化延时函数
	
	Timing_Fun_Init(10000-1, 72-1);  //100Hz  10ms
	
	Init_HMC5883L();

	/* 初始化MPU6050 */
	ret = mpu6050_init();
	if (ret != 0)
		printf("MPU6050 init failed!\r\n");
	
	// 初始化校准系统
	SensorCalibration_Init();
	
	KalmanFilter_Init(&kf);
	
	
	// 启动校准（实际应用中应通过用户交互触发）
//	SensorCalibration_StartAccCalibration();
//	printf("将设备放置X+\r\n");

//	SensorCalibration_StartGyroCalibration();
//	SensorCalibration_StartMagCalibration();
	// 模拟传感器数据（未滤波）

	
	while(1)
	{
		#if 0
		//校准
		READ_HMC5883L(&mag_raw[0], &mag_raw[1], &mag_raw[2]);
//		mpu6050_get_temperature(&temperature);
		mpu6050_get_gyroscope(&gyro_raw[0], &gyro_raw[1], &gyro_raw[2]);
		mpu6050_get_accelerometer(&acc_raw[0], &acc_raw[1], &acc_raw[2]);
		
		
		// 处理校准流程
		SensorCalibration_Process(acc_raw, gyro_raw, mag_raw);
		
		printf("acc_x: %d, acc_y: %d, acc_z: %d\r\n", acc_raw[0], acc_raw[1], acc_raw[2]);
		printf("gyr_x: %d, gyr_y: %d, gyr_z: %d\r\n", gyro_raw[0], gyro_raw[1], gyro_raw[2]);
		printf("mag_x: %d, mag_y: %d, mag_z: %d\r\n", mag_raw[0], mag_raw[1], mag_raw[2]);
		
		if(SensorCalibration_GetState() == CALIB_COMPLETED)
		{
			printf("校准完成\r\n");
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

			/* 上传相关数据信息至串口调试助手 */
	//		printf("pit: %.2f, rol: %.2f, yaw: %.2f\r\n", pit, rol, yaw);
//			printf("acc_x_cal: %d, acc_y_cal: %d, acc_z_cal: %d\r\n", acc_cal[0], acc_cal[1], acc_cal[2]);
//			printf("gyr_x_cal: %d, gyr_y_cal: %d, gyr_z_cal: %d\r\n", gyro_cal[0], gyro_cal[1], gyro_cal[2]);
//			printf("mag_x_cal: %d, mag_y_cal: %d, mag_z_cal: %d\r\n", mag_cal[0], mag_cal[1], mag_cal[2]);
			
			
			 // 打印滤波前后的数据
			printf("原始数据:\n");
			printf("加速度: [%.4f, %.4f, %.4f]\n", sensor_data[0], sensor_data[1], sensor_data[2]);
			printf("陀螺仪: [%.4f, %.4f, %.4f]\n", sensor_data[3], sensor_data[4], sensor_data[5]);
			printf("磁力计: [%.4f, %.4f, %.4f]\n", sensor_data[6], sensor_data[7], sensor_data[8]);
			
			printf("\n滤波后数据:\n");
			printf("加速度: [%.4f, %.4f, %.4f]\n", filtered_data[0], filtered_data[1], filtered_data[2]);
			printf("陀螺仪: [%.4f, %.4f, %.4f]\n", filtered_data[3], filtered_data[4], filtered_data[5]);
			printf("磁力计: [%.4f, %.4f, %.4f]\n", filtered_data[6], filtered_data[7], filtered_data[8]);
		}
		delay_ms(50);
	#endif
	
				/* 上传状态帧和传感器帧至匿名地面站V4 */
//			demo_niming_report_status((int16_t)(roll * 100), (int16_t)((pitch) * 100), (int16_t)(yaw * 100), 0, 0, 0);
//			demo_niming_report_senser(acc_cal[0], acc_cal[1], acc_cal[2], gyro_cal[0], gyro_cal[1], gyro_cal[2], mag_cal[0], mag_cal[1], mag_cal[2]);
	}
}


/**
 * @brief       发送状态帧至匿名地面站V4
 * @param       rol     : 横滚角
 *              pit     : 俯仰角
 *              yaw     : 航向角
 *              alt     : 飞行高度，单位：cm
 *              fly_mode: 飞行模式
 *              armed   : 锁定状态，0xA0：加锁 0xA1：解锁
 * @retval      无
 */
static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed)
{
    uint8_t send_buf[12];
    
    /* 横滚角 */
    send_buf[0] = (rol >> 8) & 0xFF;
    send_buf[1] = rol & 0xFF;
    /* 俯仰角 */
    send_buf[2] = (pit >> 8) & 0xFF;
    send_buf[3] = pit & 0xFF;
    /* 航向角 */
    send_buf[4] = (yaw >> 8) & 0xFF;
    send_buf[5] = yaw & 0xFF;
    /* 飞行高度 */
    send_buf[6] = (alt >> 24) & 0xFF;
    send_buf[7] = (alt >> 16) & 0xFF;
    send_buf[8] = (alt >> 8) & 0xFF;
    send_buf[9] = alt & 0xFF;
    /* 飞行模式 */
    send_buf[10] = fly_mode;
    /* 锁定状态 */
    send_buf[11] = armed;
    
    /* 状态帧的功能字为0x01 */
    demo_usart1_niming_report(0x01, send_buf, 12);
}


/**
 * @brief       发送传感器帧至匿名地面站V4
 * @param       acc_x : x轴上的加速度值
 *              acc_y : y轴上的加速度值
 *              acc_z : z轴上的加速度值
 *              gyro_x: x轴上的陀螺仪值
 *              gyro_y: y轴上的陀螺仪值
 *              gyro_z: z轴上的陀螺仪值
 *              mag_x : x轴上的磁力计值（ATK-MS6050不支持）
 *              mag_y : y轴上的磁力计值（ATK-MS6050不支持）
 *              mag_z : z轴上的磁力计值（ATK-MS6050不支持）
 * @retval      无
 */
static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z)
{
    uint8_t send_buf[18];
    
    /* x轴上的加速度值 */
    send_buf[0] = (acc_x >> 8) & 0xFF;
    send_buf[1] = acc_x & 0xFF;
    /* y轴上的加速度值 */
    send_buf[2] = (acc_y >> 8) & 0xFF;
    send_buf[3] = acc_y & 0xFF;
    /* z轴上的加速度值 */
    send_buf[4] = (acc_z >> 8) & 0xFF;
    send_buf[5] = acc_z & 0xFF;
    /* x轴上的陀螺仪值 */
    send_buf[6] = (gyro_x >> 8) & 0xFF;
    send_buf[7] = gyro_x & 0xFF;
    /* y轴上的陀螺仪值 */
    send_buf[8] = (gyro_y >> 8) & 0xFF;
    send_buf[9] = gyro_y & 0xFF;
    /* z轴上的陀螺仪值 */
    send_buf[10] = (gyro_z >> 8) & 0xFF;
    send_buf[11] = gyro_z & 0xFF;
    /* x轴上的磁力计值 */
    send_buf[12] = (mag_x >> 8) & 0xFF;
    send_buf[13] = mag_x & 0xFF;
    /* y轴上的磁力计值 */
    send_buf[14] = (mag_y >> 8) & 0xFF;
    send_buf[15] = mag_y & 0xFF;
    /* z轴上的磁力计值 */
    send_buf[16] = (mag_z >> 8) & 0xFF;
    send_buf[17] = mag_z & 0xFF;
    
    /* 传感器的功能字为0x02 */
    demo_usart1_niming_report(0x02, send_buf, 18);
}

/**
 * @brief       通过串口1发送数据至匿名地面站V4
 * @param       fun: 功能字
 *              dat: 待发送的数据（最多28字节）
 *              len: dat数据的有效位数
 * @retval      无
 */
static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len)
{
    uint8_t send_buf[32];
    uint8_t i;
    
    if (len > 28)
    {
        return;
    }
    
    send_buf[len+4] = 0;            /* 校验位清零 */
    send_buf[0] = 0xAA;             /* 帧头为0xAAAA */
    send_buf[1] = 0xAA;             /* 帧头为0xAAAA */
    send_buf[2] = fun;              /* 功能字 */
    send_buf[3] = len;              /* 数据长度 */
    for (i=0; i<len; i++)           /* 复制数据 */
    {
        send_buf[4 + i] = dat[i];
    }
    for (i=0; i<(len + 4); i++)     /* 计算校验和 */
    {
        send_buf[len + 4] += send_buf[i];
    }
    
    /* 发送数据 */
    Usart_SendString(send_buf, len + 5);
		
		
}

/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// 等待发送缓冲区为空
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			
			// 发送数据
			USART_SendData(USART1, str[i]);
	}
	
	// 等待发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}



