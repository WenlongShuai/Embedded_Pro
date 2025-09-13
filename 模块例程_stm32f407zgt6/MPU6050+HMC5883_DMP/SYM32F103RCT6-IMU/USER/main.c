#include "sys.h"
#include "delay.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "inv_mpu.h"
#include "usart.h"

extern float  angle;

static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed);
static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z);
static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len);
void Usart_SendString(uint8_t *str, uint32_t len);


int main(void)
{	
	uint8_t ret = 0;
	float pit, rol, yaw;
	int16_t acc_x, acc_y, acc_z;
	int16_t gyr_x, gyr_y, gyr_z;
	
	uart_init(115200);
	delay_init();                                   //初始化延时函数
	
	Init_HMC5883L();

	/* 初始化MPU6050 */
	ret = mpu6050_init();
	if (ret != 0)
		printf("MPU6050 init failed!\r\n");
	
	/* 初始化MPU6050 DMP */
	ret = mpu6050_dmp_init();
	if (ret != 0)
		printf("MPU6050 DMP init failed!\r\n");
	
	while(1)
	{
		// 将磁力计数据传递给DMP
    mpu_set_compass_data(mag);
		
		/* 获取ATK-MS6050 DMP处理后的数据 */
		ret  = mpu6050_dmp_get_data(&pit, &rol, &yaw);
		/* 获取ATK-MS6050加速度值 */
		ret = mpu6050_get_accelerometer(&acc_x, &acc_y, &acc_z);
		/* 获取ATK-MS6050陀螺仪值 */
		ret = mpu6050_get_gyroscope(&gyr_x, &gyr_y, &gyr_z);
		
		READ_HMC5883L();

		/* 上传相关数据信息至串口调试助手 */
		printf("pit: %.2f, rol: %.2f, yaw: %.2f\r\n", pit, rol, yaw);
		printf("acc_x: %d, acc_y: %d, acc_z: %d\r\n", acc_x, acc_y, acc_z);
		printf("gyr_x: %d, gyr_y: %d, gyr_z: %d\r\n", gyr_x, gyr_y, gyr_z);
		
		READ_HMC5883L();
		printf("angle: %.2f\r\n", angle);
		
		/* 上传状态帧和传感器帧至匿名地面站V4 */
//		demo_niming_report_status((int16_t)(rol * 100), (int16_t)((pit) * 100), (int16_t)(yaw * 100), 0, 0, 0);
//		demo_niming_report_senser(acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, 0, 0, 0);
		
		delay_ms(100);
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