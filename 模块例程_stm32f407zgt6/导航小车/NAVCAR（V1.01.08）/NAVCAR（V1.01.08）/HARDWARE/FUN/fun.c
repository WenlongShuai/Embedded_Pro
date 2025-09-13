#include "fun.h"
#include "pid.h"
#include "usart.h"
#include <math.h>
#include "wifi.h" 


struct PID YAW_PID;
struct PID DIS_PID;

extern __align(4) u8 WIFI_Tx_StrBuff[300]; 

void Get_CurrentVal(double*gps_j,double*gps_w,double*wifi_j,double*wifi_w,float*qmc5883_yaw)
{
	SensorDate sensor_date;
	double delta_x=*wifi_j-*gps_j;
	double delta_y=*wifi_w-*gps_w;
	float temp;
	static u32 timer_printf=0;
	
	sensor_date.gps_j=*gps_j;
	sensor_date.gps_w=*gps_w;
	sensor_date.qmc5883_yaw=*qmc5883_yaw;
	sensor_date.wifi_j=*wifi_j;
	sensor_date.wifi_w=*wifi_w;
	
	delta_x=delta_x*111000*cos(*gps_w*3.1416/180);
	delta_y=delta_y*111000;
	
	if(sensor_date.qmc5883_yaw<0) sensor_date.qmc5883_yaw=sensor_date.qmc5883_yaw+360;
	YAW_PID.ActualValue=sensor_date.qmc5883_yaw;
	
	if( (delta_x>0) && (delta_y>=0) ) //一象限
	{
		temp=atan(delta_y/delta_x)*180.0/3.1416;
		YAW_PID.SetValue=90-fabs(temp);
	}
	else if( (delta_x>0) && (delta_y<0) ) //二象限
	{
		temp=atan(delta_y/delta_x)*180.0/3.1416;
		YAW_PID.SetValue=90+fabs(temp);
	}
	else if( (delta_x<0) && (delta_y<0) ) //三象限
	{
		temp=atan(delta_y/delta_x)*180.0/3.1416;
		YAW_PID.SetValue=270-fabs(temp);
	}
	else if( (delta_x<0) && (delta_y>=0) ) //四象限
	{
		temp=atan(delta_y/delta_x)*180.0/3.1416;
		YAW_PID.SetValue=270+fabs(temp);
	}
	printf("gps_j:%.5lf,gps_w:%.5lf,wifi_j:%.5lf,wifi_w:%.5lf,yaw:%.2f°\r\n",*gps_j,*gps_w,*wifi_j,*wifi_w,*qmc5883_yaw);	
	printf("偏航角PID设定值为：%.2lf°，实际值为：%.2lf°\r\n",YAW_PID.SetValue,YAW_PID.ActualValue);	
	
//	delta_x=delta_x*111000*cos(*gps_w*3.1416/180);
//	delta_y=delta_y*111000;
	DIS_PID.SetValue=0.0;
	
	temp=pow( fabs(delta_x) ,2)+pow( fabs(delta_y) ,2);
	DIS_PID.ActualValue=sqrt(temp);
	printf("距离PID设定值为：%.2lfm，实际值为：%.2lfm\r\n",DIS_PID.SetValue,DIS_PID.ActualValue);	

	if((timer_printf%30)==0)
	{
		sprintf((char *)WIFI_Tx_StrBuff,"gps_j:%.5lf,gps_w:%.5lf,wifi_j:%.5lf,wifi_w:%.5lf,yaw_qmc:%.2f,yaw_st:%.2f,dis_cv:%.2lfm\r\n",*gps_j,*gps_w,*wifi_j,*wifi_w,YAW_PID.ActualValue,YAW_PID.SetValue,DIS_PID.ActualValue);		
		ESP8266_SendDate((char *)WIFI_Tx_StrBuff);
		timer_printf=0;
	}
	timer_printf++;
}

void Calculate_PID(void)
{
	#if 1
	PID_Control(&YAW_PID);
	YAW_PID.OUT=(fabs(YAW_PID.Error)<2)?0:YAW_PID.OUT;
	printf("航向PID输出为：%.2f,",YAW_PID.OUT);
	#endif
	
	#if 1
	PID_Control(&DIS_PID);
	DIS_PID.OUT=(float)(60-50)/(10-1.5)*fabs(DIS_PID.Error);
	DIS_PID.OUT=fabs(DIS_PID.Error)<1.5?0:fabs(DIS_PID.OUT);
	if(fabs(DIS_PID.Error)>=10)DIS_PID.OUT=60;
	
	if(DIS_PID.ActualValue>2000) DIS_PID.OUT=0;
	printf("距离PID输出为：%.2f\r\n",DIS_PID.OUT);
	#endif
}

void Navigation_of_Car(void)
{
	int PWM_Out_L=YAW_PID.OUT+DIS_PID.OUT;
	int PWM_Out_R=-YAW_PID.OUT+DIS_PID.OUT;
	
	PWM_Out_L=PWM_Out_L<0?0:PWM_Out_L;
	PWM_Out_L=PWM_Out_L>100?100:PWM_Out_L;
	
	PWM_Out_R=PWM_Out_R<0?0:PWM_Out_R;
	PWM_Out_R=PWM_Out_R>100?100:PWM_Out_R;
	
	printf("左轮输出为：%d，右轮输出为：%d\r\n\r\n",PWM_Out_L,PWM_Out_R);
	
	TIM_SetCompare1(TIM4,PWM_Out_L);	//修改比较值，修改占空比
	TIM_SetCompare2(TIM4,PWM_Out_R);	//修改比较值，修改占空比
}
