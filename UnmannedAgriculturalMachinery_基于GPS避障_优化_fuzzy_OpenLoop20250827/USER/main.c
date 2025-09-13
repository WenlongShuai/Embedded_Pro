#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sys.h"
#include "freeRTOS_TaskList.h"
#include "delay.h"
#include "usart.h"
#include "bluetooth.h"
#include "fuzzy_decision.h"
#include "key.h"


#if !DISMODE
#include "oled.h"
#else
#include "lcd_ili9341.h"
#include "gui.h"
#endif
#include "w25qxx.h"
#include "jy901p.h"
#include "jy901p_usart.h"

#if !GETDISMODE
#include "hc_sr04.h"
#else
#include "atk_ms53l1m.h"
#endif

#if !CARTYPE
#include "motor.h"
#else
#include "bigCarMotorControl.h"
#endif


#if defined(PID_ASSISTANT_EN) 
#include "protocol.h"
#include "pid_usart.h"
#include "timing_fun.h"
#endif


void EnableFPU(void) {
    *((volatile uint32_t *)0xE000ED88) |= (0xF << 20);
    __DSB();
    __ISB();
}

#if !CARTYPE
extern struct PID_ST pid0, pid1;
#endif

int main()
{	
	EnableFPU();   // ����FPU
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	sys_stm32_clock_init(336, 8, 2, 7);         /* ����ʱ��,168Mhz */
	delay_init(168);                            /* ��ʱ��ʼ�� */
	usart_init(84, 115200);                     /* ���ڳ�ʼ��Ϊ115200 */	
	
	#if defined(PID_ASSISTANT_EN) 
	protocol_init();
	PID_USART_Init();
	Timing_Fun_Init(1000-1, 840-1); //10ms
	#endif
	
	#if !DISMODE
	OLED_Init();  //OLED ��ʼ��
	OLED_CLS();
	#else
	Lcd_Init();    //LCD ��ʼ��
	#endif
	
	SPI_FLASH_Init();  //FLASH��ʼ��
	
	trackCenterCoordinatesInit(); // ��ʼ��ѭ���е�home����
	KEY_Init();    //�ɼ�ģʽ�³�ʼ������
	
	#if !CARTYPE
	Motor_Config();  //�����������PWM��ʼ��
	PID_Config();  //PID��ʼ��
	#endif

	Track_Init();
	freertos_setStartTask();
	
//	Flash_Read();

	#if 0
	uint8_t num = 0;
	uint8_t disBuff[50] = {0};
	uint8_t randNum = 0;
	
//	Bluetooth_USART_GPIO_Init(115200);
	KEY_Init(); 
	
	extern uint8_t keyRet;
	
//	bigCarMotorControlInit();
	
//	APP_ON_Flag = 1;
//	RC_Velocity = 300;

	while(1)
	{	
		#if defined(PID_ASSISTANT_EN) 
		/* �������ݴ��� */
		receiving_process();	
		int temp = pid0.current_point;    // ��λ����Ҫ����������ת��һ��
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &temp, 1);     // ��ͨ�� 1 ����ʵ��ֵ
		#endif
		
//		
//		
//		
////		Track();//ѭ������
//		Flash_Collect();
//		
////		uint8_t num = 0;
////		Get_Num(&num);
////		printf("num ---> %d\r\n",num);
////		Flash_Read();


//		for(int i = 0;i < 5;i++)
//		{
//			Hcsr04Start(i);	//����������ģ����
//			delay_ms(50);                           /* ��ʱ40ticks */
//		}
//		
//		Hcsr04Read();

//		num %= 4;
//		oledChangeDataDis(num, 0, 0, ultrasonic[0].distance, ultrasonic[1].distance, ultrasonic[2].distance, ultrasonic[3].distance, ultrasonic[4].distance, 0, 0);
//		num++;
	}
	#endif
	
	return 0;
}

