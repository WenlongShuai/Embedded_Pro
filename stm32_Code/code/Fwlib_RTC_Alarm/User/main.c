#include "stm32f10x.h"
#include "bsp_rtc.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_beep.h"

/*
stm32制作万年历，初始化的时间为2000.1.1 00:00:00  星期六 
通过串口可以调整RTC寄存器中的时间
*/

extern uint8_t time_flag;
extern uint8_t alarm_flag;

struct rtc_time time = {0,0,0,1,1,2000,0};

int main()
{
	uint8_t alarm_count = 0;
	
	LED_GPIO_Config();
	USART_GPIO_Init();
	Key_GPIO_Config();
	BEEP_GPIO_Config();
	NVIC_Config();
	
	
	LED_B(ON);
	
	RTC_CheckAndConfig(&time);
	
	while(1)
	{
		if(time_flag == 1)
		{
			time_flag = 0;
			time_Display(RTC_GetCounter(), &time);
			if(alarm_flag == 1)
			{
				alarm_count++;
			}
			
			if(alarm_count == 5)
			{
				alarm_flag = 0;
				alarm_count = 0;
				BEEP_Cmd(DISABLE);
			}
		}
		
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			TimeRegulate_And_SetAlarmClock(1, &time);  //串口修改时间，并把修改的时间数据放在time结构体中
			time_Adjust(&time);  //将修改的时间通过转换成UNIX时间戳存进RTC_CNT中
			BKP_WriteBackupRegister(RTC_BKP_DRx, RTC_BKP_DATA);  //RTC_BKP_DRx寄存器中写入RTC_BKP_DATA数据，表示已经对RTC的计数器进行配置过
		}
		else if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			TimeRegulate_And_SetAlarmClock(2, &time);  //串口设置闹钟，并把修改的时间数据放在time结构体中
			alarm_Adjust(&time);
		}
		
	}
	return 0;
}

