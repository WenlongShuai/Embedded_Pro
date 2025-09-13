#ifndef _GPS_
#define _GPS_
#include "mm32_device.h"
#include "hal_conf.h"
#include "headfile.h"
void uart_interrupt_handler (void);

extern char location[100];
extern double my_x,my_y;
extern double home_x,home_y;
void send_string(char* ch,int num);
double GPS_Convert(u8* a);

typedef struct{
	double x;// m
	double y;// m
}POSI_ST;
#define USART2_MAX_RECV_LEN		1024	//�����ջ����ֽ���
u8 SkyTra_Cfg_Rate(u8 Frep);
void show_gps(void);
#define home_lo 118.7115405 //����趨Ϊ����������
#define home_la 30.9083666  
void GPS_INIT(void);
void GET_MY_POSI(POSI_ST* posi);
void GET_POSI(double longitude,double latitude,POSI_ST* posi);
//--------------------------------------------------------------------------------------------------
//��������
//--------------------------------------------------------------------------------------------------
#define GPS_TAU1201_UART    (UART_2)
#define GPS_TAU1201_RX      (UART2_TX_A02)                                      // GPS RX�������ӵ���Ƭ����
#define GPS_TAU1201_TX      (UART2_RX_A03)                                      // GPS TX��������


typedef struct{
	uint16 year;  
	uint8  month; 
	uint8  day;
	uint8  hour;
	uint8  minute;
	uint8  second;
}gps_time_struct;

typedef struct{
    gps_time_struct    time;                                                    // ʱ��
    
    uint8       state;                                                          // ��Ч״̬  1����λ��Ч  0����λ��Ч
    
    uint16      latitude_degree;	                                            // ��
	uint16      latitude_cent;		                                            // ��
	uint16      latitude_second;                                                // ��
	uint16      longitude_degree;	                                            // ��
	uint16      longitude_cent;		                                            // ��
	uint16      longitude_second;                                               // ��
    
    double      latitude;                                                       // ����
    double      longitude;                                                      // γ��
    
    int8 	    ns;                                                             // γ�Ȱ��� N�������򣩻� S���ϰ���
    int8 	    ew;                                                             // ���Ȱ��� E���������� W��������
    
	float 	    speed;                                                          // �ٶȣ�����/ÿСʱ��
    float 	    direction;                                                      // ���溽��000.0~359.9 �ȣ����汱��Ϊ�ο���׼��
    
    // ������������Ϣ��GNGGA����л�ȡ
    uint8       satellite_used;                                                 // ���ڶ�λ����������
	float 	    height;                                                         // �߶�   
}gps_info_struct;

extern gps_info_struct  gps_tau1201;
extern uint8            gps_tau1201_flag;

void        gps_uart_callback           (void);

double      get_two_points_distance     (double lat1, double lng1, double lat2, double lng2);
double      get_two_points_azimuth      (double lat1, double lon1, double lat2, double lon2);

void        gps_data_parse              (void);

void        gps_init                    (void);

#endif