#include "bsp_rtc.h"

const uint8_t *week[7] = {"日", "一", "二", "三", "四", "五", "六"};

/*
配置之前的操作：
1、系统复位后，对后备寄存器和RTC的访问被禁止，这是为了防止对后备区域(BKP)的意外写操
作。执行以下操作将使能对后备寄存器和RTC的访问：
	a、设置寄存器RCC_APB1ENR的PWREN和BKPEN位，使能电源和后备接口时钟
	b、设置寄存器PWR_CR的DBP位，使能对后备寄存器和RTC的访问。
2、因为相关的可读寄存器只在RTC与APB1时钟进行重新同步的RTC时钟的上升沿被更新，
因此，若在读取RTC寄存器时，RTC的APB1接口曾经处于禁止状态，则软件首先必须等待
RTC_CRL寄存器中的RSF位(寄存器同步标志)被硬件置’1’。

RTC配置过程：
1. 查询RTOFF位，直到RTOFF的值变为’1’
2. 置CNF值为1，进入配置模式
3. 对一个或多个RTC寄存器进行写操作
4. 清除CNF标志位，退出配置模式
5. 查询RTOFF，直至RTOFF位变为’1’以确认写操作已经完成
注意事项：如果对多个寄存器进行写操作之后，退出配置模式，等待写操作完成的时候就会卡死。可能每写一个寄存器就要等待写完操作
*/
static void RTC_Config(void)
{
	//打开PWREN和BKPEN的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
	
	//使能对后备寄存器和RTC的访问
	PWR_BackupAccessCmd(ENABLE);
	
	//BKP备份域软件复位
	BKP_DeInit();
	
	#if defined(RTC_CLOCK_SOURCE_LSI)
	
	//使用内部低速时钟LSI 
	RCC_LSICmd(ENABLE);
	//等待LSI准备就绪
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	//配置RTC时钟(RTCCLK)
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	//开启或关闭RTC时钟
	RCC_RTCCLKCmd(ENABLE);
	//等待RTC与APB1时钟同步，这样RTC相关的寄存器的值才被更新，否则读出来的为0
	RTC_WaitForSynchro();
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	//设置RTC分频器，使TR_CLK=40KHz/40000-1+1=1Hz。fTR_CLK = fRTCCLK/(PRL[19:0]+1)
	RTC_SetPrescaler(40000-1);
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	//允许秒中断
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	
	#else
	//外部低速振荡器使能
	RCC_LSEConfig(RCC_LSE_ON);
	//等待LSE准备就绪
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	//配置RTC时钟(RTCCLK)
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	//开启或关闭RTC时钟
	RCC_RTCCLKCmd(ENABLE);
	//等待RTC与APB1时钟同步，这样RTC相关的寄存器的值才被更新，否则读出来的为0
	RTC_WaitForSynchro();
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	//设置RTC分频器，使TR_CLK=32.768KHz/32768-1+1=1Hz。fTR_CLK = fRTCCLK/(PRL[19:0]+1)
	RTC_SetPrescaler(RTC_PRE);
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	//允许秒中断
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
	#endif
}

void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_Init(&NVIC_InitStruct);
}

//调整计数器中的UNIX时间戳
void time_Adjust(struct rtc_time *rtc_time)
{
	uint32_t UNIX = 0;
	//RTC的初始化配置
	RTC_Config();
	
	UNIX = mktimev(rtc_time);
	
	//UNIX-TIME_ZOOM 将北京时间戳转成UNIX时间写入计数器
	RTC_SetCounter(UNIX-TIME_ZOOM);
	
	//上一次对RTC寄存器的写操作已经完成
	RTC_WaitForLastTask();
}

//检查RTC是否为备份域复位来复位或由侵入引脚事件复位，并配置RTC
void RTC_CheckAndConfig(struct rtc_time *rtc_time)
{
	//说明之前没有对RTC进行写入时间操作，或者已经被复位过，需要重新配置
	if(BKP_ReadBackupRegister(RTC_BKP_DRx) != RTC_BKP_DATA)
	{
		printf("RTC没有被配置过或者已经被备份域复位\r\n");
		time_Adjust(rtc_time);
		BKP_WriteBackupRegister(RTC_BKP_DRx, RTC_BKP_DATA);
	}
	else //说明RTC的计数器中有时间戳，那么就进行读取时间或者修改时间
	{
		printf("RTC 已经配置过，可以直接读取时间或者修改时间\r\n");
		if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)
		{
			printf("RTC 通过NRST引脚复位\r\n");
		}
		else
		{
			printf("RTC 通过上电/掉电复位\r\n");
		}
		RCC_ClearFlag();
		
		//打开PWREN和BKPEN的时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
		
		//使能对后备寄存器和RTC的访问
		PWR_BackupAccessCmd(ENABLE);
		
		#if defined(RTC_CLOCK_SOURCE_LSI)
		
		//使用内部低速时钟LSI 
		RCC_LSICmd(ENABLE);
		//等待LSI准备就绪
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
		//配置RTC时钟(RTCCLK)
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		//开启或关闭RTC时钟
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC与APB1时钟同步，这样RTC相关的寄存器的值才被更新，否则读出来的为0
		RTC_WaitForSynchro();
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		//设置RTC分频器，使TR_CLK=40KHz/40000-1+1=1Hz。fTR_CLK = fRTCCLK/(PRL[19:0]+1)
		RTC_SetPrescaler(40000-1);
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		//允许秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		
		#else
		//外部低速振荡器使能
		RCC_LSEConfig(RCC_LSE_ON);
		//等待LSE准备就绪
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		//配置RTC时钟(RTCCLK)
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		//开启或关闭RTC时钟
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC与APB1时钟同步，这样RTC相关的寄存器的值才被更新，否则读出来的为0
		RTC_WaitForSynchro();
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		//设置RTC分频器，使TR_CLK=32.768KHz/32768-1+1=1Hz。fTR_CLK = fRTCCLK/(PRL[19:0]+1)
		RTC_SetPrescaler(RTC_PRE);
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		//允许秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//上一次对RTC寄存器的写操作已经完成
		RTC_WaitForLastTask();
		#endif	
	}
}

//显示当前时间(农历、新历)
void time_Display(uint32_t timeVal, struct rtc_time *rtc_time)
{
	uint32_t BJ_TimeVal = 0;
	uint8_t str[200] = {0};
	
	//将标准时间转成北京时间
	BJ_TimeVal = timeVal + TIME_ZOOM;
	
	to_tm(BJ_TimeVal, rtc_time);
	
	if(GetChinaCalendar(rtc_time->tm_year,rtc_time->tm_mon,rtc_time->tm_mday,str))
	{
		printf("今天新历：%d%d-%d-%d\r\n",str[0],str[1],str[2],str[3]);
	}
	
	GetChinaCalendarStr(rtc_time->tm_year,rtc_time->tm_mon,rtc_time->tm_mday,str);
	printf("今天农历：%s\r\n",str);
	
	if(GetJieQiStr(rtc_time->tm_year,rtc_time->tm_mon,rtc_time->tm_mday,str))
	{
		printf("今天农历：离小寒还有05天：%s\r\n",str); 
	}
	
	printf("当前时间为：%d-%d-%d\t (星期%s)\t %d:%d:%d\r\n",rtc_time->tm_year,rtc_time->tm_mon,rtc_time->tm_mday,week[rtc_time->tm_wday],
																				rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec);
}

//通过串口调节相应时间
//使用串口发送的时候，需要在发送的字符后面加回车符
void Time_Regulate_Get(struct rtc_time *tm)
{
	uint32_t temp_num = 0;
	uint8_t day_max=0 ;
	printf("\r\n=========================设置时间==================");
		
	do 
	{
		printf("\r\n  请输入年份(Please Set Years),范围[1970~2038]，输入字符后请加回车:");
		scanf("%d",&temp_num);
		if(temp_num <1970 || temp_num >2038)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
					
		}
		else
		{	  
			printf("\n\r  年份被设置为: %d\n\r", temp_num);

			tm->tm_year = temp_num;
			break;
		}
	}while(1);


	do 
	{
		printf("\r\n  请输入月份(Please Set Months):范围[1~12]，输入字符后请加回车:");
		scanf("%d",&temp_num);
		if(temp_num <1 || temp_num >12)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
					
		}
		else
		{	  
			printf("\n\r  月份被设置为: %d\n\r", temp_num);

			tm->tm_mon = temp_num;
			break;
		}
	}while(1);
		
	/*根据月份计算最大日期*/
	switch(tm->tm_mon)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:					
				day_max = 31;
			break;
		
		case 4:
		case 6:
		case 9:
		case 11:
				day_max = 30;
			break;
		
		case 2:					
				 /*计算闰年*/
				if((tm->tm_year%4==0) &&
					 ((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&
					 (tm->tm_mon>2)) 
						{
							day_max = 29;
						} else 
						{
							day_max = 28;
						}
			break;			
	}

	do 
	{				
		printf("\r\n  请输入日期(Please Set Months),范围[1~%d]，输入字符后请加回车:",day_max);
		scanf("%d",&temp_num);
		
		if(temp_num <1 || temp_num >day_max)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
		}
		else
		{
			printf("\n\r  日期被设置为: %d\n\r", temp_num);

			tm->tm_mday = temp_num;
			break;
		}
	}while(1);
		
	do 
	{				
		printf("\r\n  请输入时钟(Please Set Hours),范围[0~23]，输入字符后请加回车:");
		scanf("%d",&temp_num);
		
		if( temp_num >23)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
		}
		else
		{
			printf("\n\r  时钟被设置为: %d\n\r", temp_num);

			tm->tm_hour = temp_num;
			break;
		}
	}while(1);

	do 
	{				
		printf("\r\n  请输入分钟(Please Set Minutes),范围[0~59]，输入字符后请加回车:");
		scanf("%d",&temp_num);
		
		if( temp_num >59)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
		}
		else
		{
			printf("\n\r  分钟被设置为: %d\n\r", temp_num);

			tm->tm_min = temp_num;
			break;
		}
	}while(1);

	do 
	{				
		printf("\r\n  请输入秒钟(Please Set Seconds),范围[0~59]，输入字符后请加回车:");
		scanf("%d",&temp_num);
		
		if( temp_num >59)
		{
			printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
		}
		else
		{
			printf("\n\r  秒钟被设置为: %d\n\r", temp_num);

			tm->tm_sec = temp_num;
			break;
		}
	}while(1);
}




