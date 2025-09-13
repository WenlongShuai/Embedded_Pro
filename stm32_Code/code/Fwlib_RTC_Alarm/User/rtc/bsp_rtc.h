#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include "stm32f10x.h"
#include "bsp_date.h"
#include "bsp_usart.h"


//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

//北京时间+8 == 格林威治时间     GMT+8 == GMT
#define TIME_ZOOM   (8*60*60)


/* 判断RTC的时间是否被配置过 */
#define RTC_BKP_DRx			BKP_DR1
#define RTC_BKP_DATA		0xA5A5


#define RTC_PRE       		0x7FFF  //32767
#define RTC_IRQ       		RTC_IRQn
#define RTC_IRQHANDLER  	RTC_IRQHandler


#define BEEP_ENABLE
#define BEEP_DISABLE


void NVIC_Config(void);
void RTC_CheckAndConfig(struct rtc_time *rtc_time);
void time_Display(uint32_t timeVal, struct rtc_time *rtc_time);
void TimeRegulate_And_SetAlarmClock(uint8_t operation, struct rtc_time *tm);
void time_Adjust(struct rtc_time *rtc_time);
void alarm_Adjust(struct rtc_time *alarm_time);

#endif /* __BSP_RTC_H__ */
