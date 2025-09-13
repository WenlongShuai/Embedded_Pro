#include "sys.h"
#include "delay.h"
#include "GY86_IIC.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS5611.h"
#include "inv_mpu.h"
#include "usart.h"

extern float Pitch,Roll,Yaw; 
extern float  angle; 

int main(void)
{	
	uart_init(115200);
	delay_init();                                   //初始化延时函数
	GY86_IIC_Init();	                    	        //初始化IIC总线
//	Init_MPU6050();                                 //初始化MPU6050 3轴加速计+3轴陀螺仪
	Init_HMC5883L();                                //初始化HMC5883L 3轴电子罗盘
	//Init_MS561101BA();                            //初始化MS5611 气压计（高度计）
	while(1)
		{
			//MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
			//MS561101BA_getPressure(MS561101BA_D1_OSR_4096);
	 	READ_HMC5883L();                              //读取并处理HMC5883L数据	
    delay_us(5);	
//    READ_MPU6050();                               //读取并处理MPU6050数据
//    delay_us(5);			
			
//		printf("Pitch = %f   Roll = %f Yaw = %f\r\n",Pitch,Roll,Yaw);
			
		printf("angle = %.2f\r\n",angle);
		delay_ms(500);
	  }
}

