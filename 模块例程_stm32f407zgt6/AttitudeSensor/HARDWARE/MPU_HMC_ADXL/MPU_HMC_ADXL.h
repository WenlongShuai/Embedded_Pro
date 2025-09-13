#ifndef __MPU_HMC_ADXL_H__
#define __MPU_HMC_ADXL_H__

#include <stdio.h>        // 标准输入输出库（用于printf）
#include <math.h>         // 数学函数库（用于atan2计算角度）
#include "SOFTWARE_IIC.h"
#include "HARDWARE_IIC.h"
#include "systick.h"
#include "MadgwickAHRS.h"
#include "W25Qxx.h"
#include "DMA.h"


//设置传感器使用硬件IIC还是软件IIC。注释 #define SOFTWARE_IIC 这条语句，表示使用硬件IIC
//#define SOFTWARE_IIC

#define UNUSED_VARIABLE(X)  ((void)(X))
	
#define RECVBUFF_SIZE             6				//接收的数据量

/*  敏感系数 */
#define ACC_SENSI    4095.9375f   //加速度敏感系数
#define GYRO_SENSI   16.38375f    //陀螺仪敏感系数
#define MAG_SENSI    6.66666667f  //磁力计敏感系数

/*  常用参数 */
#define PI   3.14f
#define DEG2RAD	   PI/180

/* HMC5883、MPU3050、ADXL345三个传感器整体初始化 */
void MPU_HMC_ADXL_Init(void);

/* IIC 写入数据\读取数据 */
unsigned char IIC_Single_Read(unsigned char Slave_Address, unsigned char REG_Address);
void IIC_Single_Write(unsigned char Slave_Address, unsigned char REG_Address, unsigned short Len,unsigned char *Data_Buf);
void IIC_Multiple_Read(unsigned char Slave_Address, unsigned char REG_Address, unsigned short size ,unsigned char *readBuf);
void IIC_Read_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size);
void IIC_Write_DMA(unsigned char Slave_Address, unsigned char REG_Address, unsigned char *data, uint16_t size);

/* 磁力计 */
/*------------------------- 宏定义 ----------------------------*/
#define HMC_SlaveAddress 0x3C // HMC5883L的I2C从机地址（注意：实际7位地址应为0x1E，0x3C是8位写地址）

#define HMC5883_CONFIG_A            	0x00
#define HMC5883_CONFIG_B            	0x01
#define HMC5883_MODE            			0x02
#define HMC5883_DATA_X_H            	0x03
#define HMC5883_DATA_X_L            	0x04
#define HMC5883_DATA_Z_H            	0x05
#define HMC5883_DATA_Z_L            	0x06
#define HMC5883_DATA_Y_H            	0x07
#define HMC5883_DATA_Y_L            	0x08
#define HMC5883_STATUS            		0x09
#define HMC5883_IDEN_A           			0x0A
#define HMC5883_IDEN_B            		0x0B
#define HMC5883_IDEN_C            		0x0C

static void Init_HMC5883(void);
unsigned char Init_HMC5883_SelfTestMode(void);
void CalibrateHMC5883(void);
void ReadData_HMC5883(float *x_raw, float *y_raw, float *z_raw);
void ReadData_HMC5883_DMA(float *x_raw, float *y_raw, float *z_raw);
double AngleToDirection(short x_raw, short y_raw, short z_raw);

/* 加速度计 */
/*------------------------- 宏定义 ----------------------------*/
#define ADXL_SlaveAddress 0xA6  //ADXL345器件读地址

#define ADXL345_DEVID            	0x00
#define ADXL345_THRESH_TAP        0x1D
#define ADXL345_OFSX            	0x1E
#define ADXL345_OFSY            	0x1F
#define ADXL345_OFSZ            	0x20
#define ADXL345_DUR            		0x21
#define ADXL345_LATENT            0x22
#define ADXL345_WINDOW            0x23
#define ADXL345_THRESH_ACT        0x24
#define ADXL345_THRESH_INACT      0x25
#define ADXL345_TIME_INACT        0x26
#define ADXL345_ACT_INACT_CTL     0x27
#define ADXL345_THRESH_FF         0x28
#define ADXL345_TIME_FF           0x29
#define ADXL345_TAP_AXES          0x2A
#define ADXL345_ACT_TAP_STATUS    0x2B
#define ADXL345_BW_RATE           0x2C
#define ADXL345_POWER_CTL         0x2D
#define ADXL345_INT_ENABLE        0x2E
#define ADXL345_INT_MAP           0x2F
#define ADXL345_INT_SOURCE        0x30
#define ADXL345_DATA_FORMAT       0x31
#define ADXL345_DATAX0            0x32
#define ADXL345_DATAX1            0x33
#define ADXL345_DATAY0            0x34
#define ADXL345_DATAY1            0x35
#define ADXL345_DATAZ0            0x36
#define ADXL345_DATAZ1            0x37
#define ADXL345_FIFO_CTL          0x38
#define ADXL345_FIFO_STATUS       0x39


static void Init_ADXL345(void);
void ReadData_ADXL345(float *x_raw, float *y_raw, float *z_raw);
void ReadData_ADXL345_DMA(float *x_raw, float *y_raw, float *z_raw);

/* MPU3050陀螺仪 */
/*------------------------- 宏定义 ----------------------------*/
//模块的A0引脚接GND，IIC的7位地址为0x68，若接到VCC，需要改为0x69
#define MPU_SlaveAddress 0xD0 		//MPU6050器件读地址

//定义MPU3050内部地址
#define MPU3050_RA_WHO            0x00
#define MPU3050_RA_PRODUCT_ID     0x01
#define MPU3050_RA_X_OFFS_H       0x0C
#define MPU3050_RA_X_OFFS_L       0x0D
#define MPU3050_RA_Y_OFFS_H       0x0E
#define MPU3050_RA_Y_OFFS_L       0x0F
#define MPU3050_RA_Z_OFFS_H       0x10
#define MPU3050_RA_Z_OFFS_L       0x11
#define MPU3050_RA_FIFO_EN        0x12
#define MPU3050_RA_AUX_VDDIO      0x13
#define MPU3050_RA_AUX_SLV_A      0x14
#define MPU3050_RA_SMPL        		0x15
#define MPU3050_RA_DLPF        		0x16
#define MPU3050_RA_INT_C        	0x17
#define MPU3050_RA_AUX_A        	0x18
#define MPU3050_RA_INT_S        	0x1A
#define MPU3050_RA_GX_H       		0x1D
#define MPU3050_RA_GX_L        		0x1E
#define MPU3050_RA_GY_H       		0x1F
#define MPU3050_RA_GY_L        		0x20
#define MPU3050_RA_GZ_H        		0x21
#define MPU3050_RA_GZ_L        		0x22
#define MPU3050_RA_AX_H        		0x23
#define MPU3050_RA_AX_L        		0x24
#define MPU3050_RA_AY_H        		0x25
#define MPU3050_RA_AY_L         	0x26
#define MPU3050_RA_AZ_H         	0x27
#define MPU3050_RA_AZ_L        		0x28
#define MPU3050_RA_FIFO_COUNT_H   0x3A
#define MPU3050_RA_FIFO_COUNT_L   0x3B
#define MPU3050_RA_FIFO_R        	0x3C
#define MPU3050_RA_USER_CTRL      0x3D
#define MPU3050_RA_PWR_M        	0x3E

static void Init_MPU3050(void);
void READ_MPU3050(void);
void ReadData_MPU3050(float *x_raw, float *y_raw, float *z_raw);
void ReadData_MPU3050_DMA(float *x_raw, float *y_raw, float *z_raw);

#endif /* __MPU_HMC_ADXL_H__ */
