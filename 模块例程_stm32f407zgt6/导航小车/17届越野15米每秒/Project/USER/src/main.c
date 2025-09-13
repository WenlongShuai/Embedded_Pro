/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				main
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
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
// *************************** ����˵�� ***************************
// 
// ������Ҫ׼����ɿƼ� MM32F3277 ���İ�һ��
// 
// ����������Ҫ׼����ɿƼ� CMSIS-DAP ���������� �� ARM ���������� һ��
// 
// �������Ǹ��չ��� ������ͬѧ����ֲʹ��
// 
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������
// 
// *************************** ����˵�� ***************************

// **************************** �궨�� ****************************
// **************************** �궨�� ****************************

// **************************** �������� ****************************

// **************************** �������� ****************************

// **************************** �������� ****************************
extern void KEY_Init();
extern volatile uint16 speed;//����ٶ�pwm����ֵ����1ms��ʱ���ж��н���б������
extern volatile  int GPS_OR_GAME;//�ɵ�ģʽ���Ǳ���ģʽ
int main(void)
{
	board_init(true);// ��ʼ�� debug �������

	LED_Init();
	KEY_Init();
//	uart_init(UART_7, 115200, UART7_TX_E08, UART7_RX_E07);//ң�ش��ڣ�����ʱ����stm32���պ�ģң�����ݲ�ͨ�����ڷ��͹�������Ϊ����û��ô�ඨʱ������PWM������
	GPS_INIT();//GPS��ʼ��
//	uart_rx_irq(UART_7, ENABLE);//ң�ؽ����ж�
	IMU_INIT();//���������Ǻʹ����Ƶĳ�ʼ��
	lcd_init();//��Ļ
	Remote_Ctrl_Init();//�������pwm��ʼ���Լ����pwm��ʼ��
	EEPROM_INIT();//ģ��IIC��ʼ��
	Track_INIT();//ѭ����ʼ��
//	EEPROM_TEST(); EEPROM��д����
//	EEPROM_READ(); ��ȡEEPROM�ڵ����ݵ�����
	
	while(1)
	{
//		Remote_Ctrl(); �ú���Ϊң�ؿ��ƣ�����������޹�

		if(GPS_OR_GAME == 1){//GPS_OR_GAME��ʼ���Ƕ�ȡIO�����һ�����뿪������ת����ѭ�����ǲɵ�
//			IMU_SHOW(); �ú���������ʾ����ǰ�����Լ����������ݣ����������ݿ�������У׼
			Track();//ѭ������
		}
		else {
			EEPROM_COLLECT();//�ɵ㺯��
		}     
		gps_data_parse();//gps���ݽ���
        
        
        
        
        
//      ����Ϊ����ʱ��ӵ��������        
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

// **************************** �������� ****************************
