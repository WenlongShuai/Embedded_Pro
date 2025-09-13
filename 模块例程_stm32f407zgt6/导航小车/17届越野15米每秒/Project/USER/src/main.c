/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				main
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#include "headfile.h"
#include "IST8310_IIC.h"
#include "led.h"
#include "GPS.h"
#include "ist8310driver.h"
#include "ist8310driver_middleware.h"
#include <math.h>
#include "TIM.h"
#include "IMU.h"
#include "remote.h"
#include "pwm.h"
#include "Track.h"
#include "EEPROM.h"
#include <stdio.h> 
// *************************** 例程说明 ***************************
// 
// 测试需要准备逐飞科技 MM32F3277 核心板一块
// 
// 调试下载需要准备逐飞科技 CMSIS-DAP 调试下载器 或 ARM 调试下载器 一个
// 
// 本例程是个空工程 用来给同学们移植使用
// 
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完
// 
// *************************** 例程说明 ***************************

// **************************** 宏定义 ****************************
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************

// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************
extern void KEY_Init();
extern volatile uint16 speed;//电机速度pwm给定值，在1ms定时器中断中进行斜坡启动
extern volatile  int GPS_OR_GAME;//采点模式还是比赛模式
int main(void)
{
	board_init(true);// 初始化 debug 输出串口

	LED_Init();
	KEY_Init();
//	uart_init(UART_7, 115200, UART7_TX_E08, UART7_RX_E07);//遥控串口，调试时我用stm32接收航模遥控数据并通过串口发送过来，因为板子没那么多定时器用于PWM捕获了
	GPS_INIT();//GPS初始化
//	uart_rx_irq(UART_7, ENABLE);//遥控接收中断
	IMU_INIT();//包括陀螺仪和磁力计的初始化
	lcd_init();//屏幕
	Remote_Ctrl_Init();//电机调速pwm初始化以及舵机pwm初始化
	EEPROM_INIT();//模拟IIC初始化
	Track_INIT();//循迹初始化
//	EEPROM_TEST(); EEPROM读写测试
//	EEPROM_READ(); 读取EEPROM内的数据到串口
	
	while(1)
	{
//		Remote_Ctrl(); 该函数为遥控控制，与比赛任务无关

		if(GPS_OR_GAME == 1){//GPS_OR_GAME起始就是读取IO，外接一个拨码开关用于转换是循迹还是采点
//			IMU_SHOW(); 该函数可以显示车身当前航向以及磁力计数据，磁力计数据可以用于校准
			Track();//循迹函数
		}
		else {
			EEPROM_COLLECT();//采点函数
		}     
		gps_data_parse();//gps数据解析
        
        
        
        
        
//      以下为调试时添加的输出函数        
//		show_gps();
//		lcd_showchar(50,50,'x');
//		lcd_showstr(40,5,"HHOP");
//		get_icm20602_accdata_spi();
//		get_icm20602_gyro_spi();
//		printf("%f %f %f\r\n",160.f*icm_acc_x/0xffff,160.f*icm_acc_y/0xffff,160.f*icm_acc_z/0xffff);
//		printf("%.2f %.2f\r\n",180 - 180.*INS_angle[0]/3.14f,mag[0]);
//		printf("%d %d %d\r\n",remote_data.mode_pwm,remote_data.forword_speed,remote_data.turn_pwm);
//		TIM_SetCompare2(TIM1, speed);
//		GPIO_ResetBits(GPIOE,GPIO_Pin_9);
//			SERVO_SET(-1000);
//		printf("%lf %lf\r\n",gps_tau1201.longitude,gps_tau1201.latitude);
	}
}

// **************************** 代码区域 ****************************
