#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "key.h"     		
#include "usart3.h"
#include "gps.h"
#include "myiic.h"
#include "press.h"
#include "usart2.h"
#include "usart6.h"
#include "sim.h" 
#include "yaw.h" 
#include "wifi.h"
#include "pwm.h"
#include "pid.h"
#include "fun.h"
#include "filter.h" 



u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					
u8 CompassBuff[200];
u8 *PcompassBuff=NULL;

nmea_msg gpsx; 											//GPS信息
yaw_msg QMC_5883L_Val;
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
	  
void Gps_Msg_Show(void);

__align(4) u8 WIFI_Tx_StrBuff[300]; 




int main(void)
{        	
	float mimi;
	extern long Pa;
	u16 i,rxlen;
	u8 lenx;
	u32 timer_printf=0;
	u8 key=0;
	u8 status=0;
	extern unsigned char ucRxFinish6;
	double WIFI_JD;
	double WIFI_WD;
	double GPS_JD;
	double GPS_WD;
	double GPS_JD_L;
	double GPS_WD_L;
	double YAW_L;
	FilterPram JD_FilterPram;
	FilterPram WD_FilterPram;
	
	extern struct PID YAW_PID;
	extern struct PID DIS_PID;
	
	void (*JD_Filter)(FilterPram*,double,double*,u8,u8);
	void (*WD_Filter)(FilterPram*,double,double*,u8,u8);
	JD_Filter=SortAver_Filter_fp;
	//WD_Filter=SortAver_Filter;
	
	KEY_Init();
	LED_Init();
	TIM4_PWM_Init(100-1,84-1); //0.1ms 10KHz
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	delay_ms(3000);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart2_init(115200);  //串口3初始化为115200
	usart3_init(115200);  //初始化串口3波特率为384200
	usart6_init(115200);
	LED0=1;
	LED1=1;
	
	PID_Init(&YAW_PID,0.06,0,0.001,100,-100); //P:0.42  //0.22,0.001,0.0001  
	PID_Init(&DIS_PID,4.5,2.5,0,0,-100); //0.5,0.015,0.008
	
	if(SkyTra_Cfg_Rate(5)!=0)	
	{
		printf("卫星正在初始化...\r\n");
		do
		{
			usart3_init(9600);			
	  	    SkyTra_Cfg_Prt(3);			
			usart3_init(38400);			
			key=SkyTra_Cfg_Tp(100000);	
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);
	    printf("卫星初始化成功!\r\n");
		delay_ms(500);
	}

	QMC_5883L_Init();
	
	ESP8266_Init();
	
	while(1) 
	{		
		
		delay_ms(100);		

#if 1		
		if(USART2_RX_STA&0X8000)	
		{
			rxlen=USART2_RX_STA&0X7FFF;	
			for(i=0;i<rxlen;i++)CompassBuff[i]=USART2_RX_BUF[i];	   
 			USART2_RX_STA=0;		   	
			CompassBuff[i]='\0';
			PcompassBuff=CompassBuff;		
			YAW_Analysis(&QMC_5883L_Val,CompassBuff);
			//printf("实际的偏航角为：%.2f°\r\n",QMC_5883L_Val.yaw);
			SortAver_Filter_YAW(QMC_5883L_Val.yaw,&YAW_L,8);
			//printf("滤波后的偏航角为：%.2lf°\r\n",YAW_L);
 		}
#endif
	
		
#if 1		
		if(USART3_RX_STA&0X8000)		
		{
			rxlen=USART3_RX_STA&0X7FFF;	
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
 			USART3_RX_STA=0;		   	
			USART1_TX_BUF[i]=0;		
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);
			//Gps_Msg_Show();
			GPS_JD=gpsx.longitude/100000.0;
			GPS_WD= gpsx.latitude/100000.0;
			//(*JD_Filter)(&JD_FilterPram,GPS_JD,&GPS_JD_L,8,1);
			//(*WD_Filter)(&WD_FilterPram,GPS_WD,&GPS_WD_L,8,1);
			SortAver_Filter_WD(GPS_WD,&GPS_WD_L,8);	
			SortAver_Filter_JD(GPS_JD,&GPS_JD_L,8);		
			//printf("实际的经度为：%.5lf°，纬度为：%.5lf°\r\n",GPS_JD,GPS_WD);
			//printf("滤波后的经度为：%.5lf°，纬度为：%.5lf°\r\n",GPS_JD_L,GPS_WD_L);
 		}
#endif		

#if 1			
		Get_Value_From_WIFI(&ucRxFinish6,&WIFI_JD,&WIFI_WD);
		//printf("设定的经度为：%.5lf°，纬度为：%.5lf°\r\n\r\n",WIFI_JD,WIFI_WD);			
#endif		
				
#if 0					
		if((timer_printf%50)==0)
		{
			sprintf((char *)WIFI_Tx_StrBuff,"CV jd:%.5lf,wd:%.5lf,yaw:%.2lf\r\n",GPS_JD_L,GPS_WD_L,YAW_L);		
			ESP8266_SendDate((char *)WIFI_Tx_StrBuff);
			printf("CV jd:%.5lf,wd:%.5lf,yaw:%.2lf\r\n",GPS_JD_L,GPS_WD_L,YAW_L);	
			delay_ms(1000);
			sprintf((char *)WIFI_Tx_StrBuff,"SV jd:%.5lfwd：%.5lf\r\n\r\n",WIFI_JD,WIFI_WD);		
			ESP8266_SendDate((char *)WIFI_Tx_StrBuff);
			printf("SV jd:%.5lf,wd：%.5lf\r\n\r\n",WIFI_JD,WIFI_WD);		
			
			timer_printf=0;
		}	
		timer_printf++;	
#endif


#if 1			
		Get_CurrentVal(&GPS_JD,&GPS_WD,&WIFI_JD,&WIFI_WD,&QMC_5883L_Val.yaw);
#endif	
#if 1		
		Calculate_PID();
#endif
#if 1	
		Navigation_of_Car();			
#endif			

#if 0		
		sprintf((char *)WIFI_Tx_StrBuff,"设定的经度为：%.5lf°，纬度为：%.5lf°",WIFI_JD,WIFI_WD);		
		ESP8266_SendDate((char *)WIFI_Tx_StrBuff);
#endif
		
#if 1			
		if((lenx%10)==0)
		{
			LED0=!LED0;
			lenx=0;
		}	
		lenx++;			
#endif		
		

#if 0			

TIM_SetCompare1(TIM4,80);	//修改比较值，修改占空比
TIM_SetCompare2(TIM4,80);	//修改比较值，修改占空比		
		
#endif

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		

		
//		key=KEY_Scan(0);
//		if(key==WKUP_PRES)
//		{
//			LED1=1;
//			delay_ms(200);
//			status=si900a_sms_test(MSISDN);	//发送短信测试，请修改宏定义标识符MSISDN
//			if(status) printf("消息发送失败!\r\n");
//			else       printf("消息发送成功!\r\n"); 
// 		}
//		
//		key=ExKey_Scan(0);
//		if(key==EXKEY_PRES)
//		{
//			LED1=1;
//			delay_ms(200);
//			status=si900a_sms_test(MSISDN);	//发送短信测试，请修改宏定义标识符MSISDN
//			if(status) printf("消息发送失败!\r\n");
//			else       printf("消息发送成功!\r\n"); 
// 		}			
		
	}
}







void Gps_Msg_Show(void)
{
 	float tp;		    	 
	tp=gpsx.longitude;	   
	printf("Longitude:%.5f %1c \r\n",tp/=100000,gpsx.ewhemi);	//得到经度字符串
		   
	tp=gpsx.latitude;	   
	printf("Latitude:%.5f %1c \r\n",tp/=100000,gpsx.nshemi);	//得到纬度字符串
 
//	tp=gpsx.altitude;	   
// 	sprintf((char *)dtbuf,"Altitude:%.1fm",tp/=10);	    			//得到高度字符串
//    printf("%s\r\n",dtbuf);
	
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h",tp/=1000);		    		//得到速度字符串	 
 	printf("%s\r\n",dtbuf);
	
	if(gpsx.fixmode<=3)														//定位状态
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  printf("%s\r\n",dtbuf);	   
	}	 	   

	sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//可见GPS卫星数
 	printf("%s\r\n",dtbuf);
	
	sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//可见北斗卫星数
 	printf("%s\r\n",dtbuf);
	
//	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
//	printf("%s\r\n",dtbuf);		    
//	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间
//  printf("%s\r\n\r\n\r\n",dtbuf);		  
}






