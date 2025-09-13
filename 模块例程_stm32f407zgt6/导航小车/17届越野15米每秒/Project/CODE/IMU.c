/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       INS_task.c/h
  * @brief      use bmi088 to calculate the euler angle. no use ist8310, so only
  *             enable data ready pin to save cpu time.enalbe bmi088 data ready
  *             enable spi DMA to save the time spi transmit
  *             ��Ҫ����������bmi088��������ist8310�������̬���㣬�ó�ŷ���ǣ�
  *             �ṩͨ��bmi088��data ready �ж�����ⲿ�������������ݵȴ��ӳ�
  *             ͨ��DMA��SPI�����ԼCPUʱ��.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V2.0.0     Nov-11-2019     RM              1. support bmi088, but don't support mpu6500
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "IMU.h"
#include "TIM.h"
#include "MahonyAHRS.h"
#include "math.h"
#include "ist8310driver.h"
#include "ist8310driver_middleware.h"
#include "headfile.h"
#include "IST8310_IIC.h"
#include "remote.h"
#define PI 3.1415926
void AHRS_init(fp32 quat[4], fp32 accel[3], fp32 mag[3]);
void AHRS_update(fp32 quat[4], fp32 time, fp32 gyro[3], fp32 accel[3], fp32 mag[3]);
void get_angle(fp32 quat[4], fp32 *yaw, fp32 *pitch, fp32 *roll);

volatile uint8_t gyro_update_flag = 0;
volatile uint8_t accel_update_flag = 0;
volatile uint8_t accel_temp_update_flag = 0;
volatile uint8_t mag_update_flag = 0;
volatile uint8_t imu_start_dma_flag = 0;

fp32 INS_quat[4] = {0.0f, 0.0f, 0.0f, 0.0f};
fp32 INS_angle[3] = {0.0f, 0.0f, 0.0f};      //euler angle, unit rad.ŷ���� ��λ rad
fp32 accel[3],gyro[3],mag[3];

void IMU_data_get(void){
	get_icm20602_accdata_spi();
	get_icm20602_gyro_spi();
	ist8310_read_mag(mag);
	accel[0] = 160.f*icm_acc_x/0xffff;
	accel[1] = 160.f*icm_acc_y/0xffff;
	accel[2] = 160.f*icm_acc_z/0xffff;
	gyro[0] = 4000.f*icm_gyro_x/0xffff;
	gyro[1] = 4000.f*icm_gyro_y/0xffff;
	gyro[2] = 4000.f*icm_gyro_z/0xffff;
	gyro[0] = PI*gyro[0]/180. + 0.031958;//�������Ϊ������У׼����ֵ�������вⶨ���������˵��
	gyro[1] = PI*gyro[1]/180. + 0.009588;//�������Ϊ������У׼����ֵ�������вⶨ���������˵��
	gyro[2] = PI*gyro[2]/180. - 0.011718;//�������Ϊ������У׼����ֵ�������вⶨ���������˵��
	mag[0] = mag[0] + 9;//Ҫ�õ��������ֵ��������ֶ�У׼�����������������ڿտ�������������ˮƽ��תһ�ܣ��۲�����������ݷ������ֵ����Сֵ֮�Ͳ�Ϊ0��˵�����ڹ̶�ƫ����� -(max+min)/2 ��Ϊ����ֵ���ڴ˴�+������ֵ
	mag[1] = mag[1] - 1;
//	-0.003196 -0.004261 -0.020240      ����������Ϊ������У׼����ֵ�������вⶨ���������˵��
}
void IMU_INIT(void){
	ist8310_init();//�شżƳ�ʼ��
	icm20602_init_spi();//�����ǳ�ʼ��
	IMU_TIM_INIT();//1ms��ʱ���ж�
	IMU_data_get();//��ȡһ������
	AHRS_init(INS_quat, accel, mag);//��Ԫ����ʼ��
}
/**
  * @brief          imu task, init bmi088, ist8310, calculate the euler angle
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
/**
  * @brief          imu����, ��ʼ�� bmi088, ist8310, ����ŷ����
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
uint16 time_stamp = 0;
extern uint16 speed;
extern	int mode;
volatile  int GPS_OR_GAME = 1; 

//��̬����������1ms��ʱ���ж���ִ��
void INS_task(void)
{
	int speed_max;
	IMU_data_get();
	AHRS_update(INS_quat, 0.001f, gyro, accel, mag);
	get_angle(INS_quat, INS_angle + INS_YAW_ADDRESS_OFFSET, INS_angle + INS_PITCH_ADDRESS_OFFSET, INS_angle + INS_ROLL_ADDRESS_OFFSET);
	if(time_stamp < 10000){
		time_stamp++;
	}
	else if(time_stamp >= 10000){
		time_stamp = 0;
	}
	mode = get_mode();
	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12) == 1){
		speed_max = 990;
	}
	else{
		speed_max = 990;
	}
//	if(time_stamp%2 == 0 && mode == 1){
//		if(speed < speed_max ){
//			speed++;
//		}
//	}
	if(mode == 1){
		if(speed < speed_max ){
			speed++;
		}
	}
//	printf("%d\r\n",GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9));
	GPS_OR_GAME = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9);
}

void AHRS_init(fp32 quat[4], fp32 accel[3], fp32 mag[3])
{
    quat[0] = 1.0f;
    quat[1] = 0.0f;
    quat[2] = 0.0f;
    quat[3] = 0.0f;

}

void AHRS_update(fp32 quat[4], fp32 time, fp32 gyro[3], fp32 accel[3], fp32 mag[3])
{
    MahonyAHRSupdate(quat, gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2], mag[0], mag[1], mag[2]);
}
void get_angle(fp32 q[4], fp32 *yaw, fp32 *pitch, fp32 *roll)
{
    *yaw = atan2f(2.0f*(q[0]*q[3]+q[1]*q[2]), 2.0f*(q[0]*q[0]+q[1]*q[1])-1.0f);
    *pitch = asinf(-2.0f*(q[1]*q[3]-q[0]*q[2]));
    *roll = atan2f(2.0f*(q[0]*q[1]+q[2]*q[3]),2.0f*(q[0]*q[0]+q[3]*q[3])-1.0f);
}
fp32 mag_max0 = 0,mag_min0 = 0;
fp32 mag_max1 = 0,mag_min1 = 0;

//TFT��ʾ���������ݺ͵شż�����
void IMU_SHOW(void){
	char str0[10],str1[10],str2[10],str3[10],str4[10];
	char str5[10],str6[10];
	sprintf(str0,"%.2f",mag[0]);
		sprintf(str1,"%.2f",mag[1]);
		sprintf(str2,"%.2f",(180 - 180.*INS_angle[0]/3.14f));//180 - 180.*INS_angle[0]/3.14f ��Ϊ�ҵĵشż��ǵ��Ű�װ�ģ���ֲʱ���е���
		lcd_showstr(0,1,str0);
		lcd_showstr(50,1,str1);
		lcd_showstr(10,2,str2);
		systick_delay_ms(10);
		if(mag[1] > mag_max1){
			mag_max1 = mag[1];
		}
		if(mag[1] < mag_min1){
			mag_min1 = mag[1];
		}
		if(mag[0] > mag_max0){
			mag_max0 = mag[0];
		}
		if(mag[0] < mag_min0){
			mag_min0 = mag[0];
		}
		sprintf(str3,"%.2f",mag_max0);
		sprintf(str4,"%.2f",mag_min0);
		sprintf(str5,"%.2f",mag_max1);
		sprintf(str6,"%.2f",mag_min1);
		lcd_showstr(0,3,str3);
		lcd_showstr(0,4,str4);
		lcd_showstr(50,3,str5);
		lcd_showstr(50,4,str6);

}
