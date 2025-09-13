#ifndef __HI600_H__
#define __HI600_H__

#include <math.h>
#include "sys.h"

typedef struct{
	double x;// m
	double y;// m
}POSI_ST;

//GPS NMEA-0183协议重要参数结构体定义 
//卫星信息
__packed typedef struct  
{										    
 	u8 num;		//卫星编号
	float eledeg;	//卫星仰角
	float azideg;	//卫星方位角
	float sn;		//信噪比		   
}nmea_slmsg; 


//北斗 NMEA-0183协议重要参数结构体定义 
//卫星信息
__packed typedef struct  
{	
 	u8 beidou_num;		//卫星编号
	float beidou_eledeg;	//卫星仰角
	float beidou_azideg;	//卫星方位角
	float beidou_sn;		//信噪比		   
}beidou_nmea_slmsg; 


//UTC时间信息
__packed typedef struct  
{										    
 	u16 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
}nmea_utc_time; 


//NMEA 0183 协议解析后数据存放结构体
__packed typedef struct  
{										    
 	u8 svnum;					//可见GPS卫星数
	u8 beidou_svnum;					//可见GPS卫星数
	nmea_slmsg slmsg[12];		//最多12颗GPS卫星
	beidou_nmea_slmsg beidou_slmsg[12];		//暂且算最多12颗北斗卫星
	nmea_utc_time utc;			//UTC时间
	double latitude;				//纬度 
	u8 nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	double longitude;			    //经度
	u8 ewhemi;					//东经/西经,E:东经;W:西经
	u8 gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
 	u8 posslnum;				//用于定位的GPS卫星数,0~12.
 	u8 possl[12];				//用于定位的卫星编号
	u8 fixmode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
	float pdop;					//位置精度因子 0~50.0
	float hdop;					//水平精度因子 0~50.0
	float vdop;					//垂直精度因子 0~50.0 
	float altitude;			 	//海拔高度
	float speed;					//地面速率  单位:1公里/小时	 
}nmea_msg;


//#define home_lo 106.6200674 //最好设定为场地最中央
//#define home_la 29.6855516  

#define HIP_USARTx 					USART2
#define HIP_USART_RCC				RCC_APB1Periph_USART2
#define HIP_USART_RCC_GPIO	RCC_AHB1Periph_GPIOD

#define HIP_USART_RX_GPIO 	GPIOD
#define HIP_USART_RX_PIN		GPIO_Pin_6
#define HIP_RX_PinSourcex 	GPIO_PinSource6
#define HIP_RX_AF 					GPIO_AF_USART2

#define HIP_USART_TX_GPIO		GPIOD
#define HIP_USART_TX_PIN		GPIO_Pin_5
#define HIP_TX_PinSourcex 	GPIO_PinSource5
#define HIP_TX_AF 					GPIO_AF_USART2

#define HIP_USART_IRQn			USART2_IRQn

#define HIP_USART_REC_LEN  			128  	//定义最大接收字节数 128

void hi600_Init(void);
void USART2_Send_BufferData(const u8 *buffer, u16 len);
void hi600_Config_OutRate(u8 Frep);
void hi600_Config_Baudrate(u32 baud_id);

void GPS_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_BDGSV_Analysis(nmea_msg *gpsx,u8 *buf);
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf);

void GeodeticToCartesian(double longitude,double latitude,double temp[]);
void Get_POSI(double longitude,double latitude,POSI_ST* posi);
void Get_My_POSI(POSI_ST* posi);
void trackCenterCoordinatesInit(void);
void collectCenterCoordinatesInit(void);


#endif /* __HI600_H__ */
