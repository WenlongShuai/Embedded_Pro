#ifndef __HIP600D_H__
#define __HIP600D_H__

#include "sys.h"


//GNSS类型
typedef enum
{
	GNGGA,
	GNGSA,
	GPGSV,
	GAGSV,
	GBGSV,
	GNRMC,
	GNVTG,
	GNENU
}GNSSType;

typedef struct
{
	u8 Name[10];  //类型名称
	float UTC;    //UTC时间
	float Latitude;  //纬度
	u8 LatitudeDir; //纬度方向
	float Longitude;  //经度
	u8 LongitudeDir;  //经度方向
	u8 QualityIndicator;  //GPS 质量指示器
	u8 SatellitesNum;   //卫星数量
	float HorizontalDilution;  //水平精度因子
	float mean_sea_leve;  //平均海平面
	u8 mean_sea_leveUnit; //平均海平面单位（M）
	float GeoidalSeparation;  //平均海平面差距
	u8 GeoidalSeparationUnit;  //平均海平面差距单位
	float DifferentialGPSData;  //差分GPS数据
	u8 DifferentialReferenceID;  //差分参考ID
}GNGGAStruct;
	
typedef struct
{
	u8 Name[10];  //类型名称
	u8 Mode1;  //模式，切换2D/3D
	u8 Mode2;  //模式，2D/3D
	u8 IDNum[12];  //解决方案中使用的卫星的ID号
	float PDOP;
	float HDOP;
	float VDOP;
	u8 GNSSSystemID;  //GNSS系统ID
}GNGSAStruct;

typedef struct
{
	u8 Name[10];  //类型名称
	u8 TotalNum;  //总数
	u8 SentenceNum;  //句子数量
	u16 SatellitesTotalNum;  //卫星总数
	u16 SatelliteID;  //卫星识别码
	u16 ElevationAngle;  //仰角，最大90度
	u8 AzimuthAngle;  //方位角，0-359
	u8 SNR;  //信噪比
	u8 SV1[3];
	u8 SV2[4];
	u8 SignalID;  //信号ID
}GP_GA_GBGSVStruct;

typedef struct
{
	u8 Name[10];
	u8 UTC[11];
	u8 Status;  //定位状态
	double Latitude;  //纬度
	u8 LatitudeDir;   //纬度方向
	double Longitude;  //经度
	u8 LongitudeDir; //经度方向
	float SpeedOverGround; //地面速度
	float CourseOverGround;  //地面航向
	u8 Date[6];  //日期，日月年
	float MagneticVariation; //磁偏角
	u8 MagneticVariationDir; //磁偏角方向
	u8 ModeIndicator;  //模式指示
	u8 NavigationalStatus;  //航向状态
}GNRMCStruct;

typedef struct
{
	u8 Name[10];
	float CourseGround1;  //对面航线
	u8 DegreesTrue;   //度数
	float CourseGround2;  //地面航线
	u8 DegreesMagnetic;   //磁度
	float SpeedGround1;  //对地速度
	u8 SpeedGroundUnit1;  //节
	float SpeedGround2;   //地面速度
	u8 SpeedGroundUnit2;   //km/hr 
	u8 ModeIndicator;  //状态指示器
}GNVTGStruct;


#define HIP_USARTx 					USART3
#define HIP_USART_RCC				RCC_APB1Periph_USART3
#define HIP_USART_RCC_GPIO	RCC_AHB1Periph_GPIOB

#define HIP_USART_RX_GPIO 	GPIOB
#define HIP_USART_RX_PIN		GPIO_Pin_11
#define HIP_RX_PinSourcex 	GPIO_PinSource11
#define HIP_RX_AF 					GPIO_AF_USART3

#define HIP_USART_TX_GPIO		GPIOB
#define HIP_USART_TX_PIN		GPIO_Pin_10
#define HIP_TX_PinSourcex 	GPIO_PinSource10
#define HIP_TX_AF 					GPIO_AF_USART3

#define HIP_USART_IRQn			USART3_IRQn

#define HIP_USART_REC_LEN  			128  	//定义最大接收字节数 200

extern u8  HIP_USART_RX_BUF[HIP_USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//接收状态
extern u8 HIP_USART_RX_STA_LINE;    //一行数据标志	
extern u8 HIP_USART_RX_STA_FRAM;   //一帧数据标志
extern u16 HIP_USART_RX_STA_COUNT;

void hip600d_USART_GPIO_Init(uint32_t bound);

void parseNavigationData(u8 *dataStr);
void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount);
int myFindString(u8 *dataStr, u8 *subStr, u8 *targetStr);

#endif /* __HIP600D_H__ */
