#ifndef __HI600_H__
#define __HI600_H__

#include <math.h>
#include "sys.h"

typedef struct{
	double x;// m
	double y;// m
}POSI_ST;

//GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� 
//������Ϣ
__packed typedef struct  
{										    
 	u8 num;		//���Ǳ��
	float eledeg;	//��������
	float azideg;	//���Ƿ�λ��
	float sn;		//�����		   
}nmea_slmsg; 


//���� NMEA-0183Э����Ҫ�����ṹ�嶨�� 
//������Ϣ
__packed typedef struct  
{	
 	u8 beidou_num;		//���Ǳ��
	float beidou_eledeg;	//��������
	float beidou_azideg;	//���Ƿ�λ��
	float beidou_sn;		//�����		   
}beidou_nmea_slmsg; 


//UTCʱ����Ϣ
__packed typedef struct  
{										    
 	u16 year;	//���
	u8 month;	//�·�
	u8 date;	//����
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
}nmea_utc_time; 


//NMEA 0183 Э����������ݴ�Žṹ��
__packed typedef struct  
{										    
 	u8 svnum;					//�ɼ�GPS������
	u8 beidou_svnum;					//�ɼ�GPS������
	nmea_slmsg slmsg[12];		//���12��GPS����
	beidou_nmea_slmsg beidou_slmsg[12];		//���������12�ű�������
	nmea_utc_time utc;			//UTCʱ��
	double latitude;				//γ�� 
	u8 nshemi;					//��γ/��γ,N:��γ;S:��γ				  
	double longitude;			    //����
	u8 ewhemi;					//����/����,E:����;W:����
	u8 gpssta;					//GPS״̬:0,δ��λ;1,�ǲ�ֶ�λ;2,��ֶ�λ;6,���ڹ���.				  
 	u8 posslnum;				//���ڶ�λ��GPS������,0~12.
 	u8 possl[12];				//���ڶ�λ�����Ǳ��
	u8 fixmode;					//��λ����:1,û�ж�λ;2,2D��λ;3,3D��λ
	float pdop;					//λ�þ������� 0~50.0
	float hdop;					//ˮƽ�������� 0~50.0
	float vdop;					//��ֱ�������� 0~50.0 
	float altitude;			 	//���θ߶�
	float speed;					//��������  ��λ:1����/Сʱ	 
}nmea_msg;


//#define home_lo 106.6200674 //����趨Ϊ����������
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

#define HIP_USART_REC_LEN  			128  	//�����������ֽ��� 128

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
