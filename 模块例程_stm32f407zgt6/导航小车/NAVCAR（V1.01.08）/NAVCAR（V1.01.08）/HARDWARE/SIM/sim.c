#include "sim.h" 
#include "delay.h"	   
#include "usart.h"
#include "usart2.h"
#include "string.h" 
#include "press.h"
#include "gps.h" 

extern long Pa;
extern nmea_msg gpsx;

//ATK-SIM800C �������(���Ų��ԡ����Ų��ԡ�GPRS���ԡ���������)���ô���
//SIM800C���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)  //���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��SIM800C��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART2->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);  //��������
	
	if(ack&&waittime)		        //��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	        //�ȴ�����ʱ
		{ 
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack))break;//�õ���Ч���� 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}



u8 sim900a_check_status(void)
{
	if(sim900a_send_cmd("AT","OK",200)) return 1;//����ATָ��жϷ����Ƿ���OK��
    if(sim900a_send_cmd("AT+CPIN?","OK",200)) return 2;// ���SIM��ʧ�ܡ�����û�в���SIM����	
    if(sim900a_send_cmd("AT+CGMI","OK",200))return 3;//��ѯ������Ӫ�̣�	
	return 0;	
}



u8 si900a_sms_test(u8* msisdn)
{
	u8 status=0;
	char cmd[20];
	
	status=sim900a_check_status();
	if(status) return status; 
	
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 4;			//�����ı�ģʽ 
	
	if(sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200))return 5;	//����TE�ַ���ΪUCS2

	sprintf((char*)cmd,"AT+CMGS=\"%s\"",msisdn);//�����ʽ��AT+CMGS="XXX"
	
	if(sim900a_send_cmd((u8*)cmd,">",200))return 6;	//���ö���Ϣ�ı�ģʽ���� 
	
	u2_printf("Current location information : %.3f%1c , %.3f%1c , altitude is %.1fm , and underwater pressure is %.3fKPa . ",gpsx.longitude/100000.0,gpsx.ewhemi,gpsx.latitude/100000.0,gpsx.nshemi,gpsx.altitude/10.0,Pa/1000.0);
	   printf("\r\n��ǰλ����Ϣ��%.3f%1c��%.3f%1c�����θ߶�%.1fm��ˮѹ%.3fKPa��\r\n\r\n",gpsx.longitude/100000.0,gpsx.ewhemi,gpsx.latitude/100000.0,gpsx.nshemi,gpsx.altitude/10.0,Pa/1000.0);
	//u2_printf("Dbb is %d years old,and today's temperature is %.2fC,now is %2d:%d ,high is %.3fm,Good Night Dbb",27,34.5678,1,30,154.6789);
	
	if(sim900a_send_cmd((u8*)0X1A,"+CMGS:",1000)) return 7;//���ͽ�����
	
  return 0;
}



/*
u8 si900a_sms_test(u8* msisdn)
{
	u8 status=0;
	char cmd[20];
	
	status=sim900a_check_status();
	if(status) return status; 
	
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 4;			//�����ı�ģʽ
	
	if(sim900a_send_cmd("AT+CSMP=17,167,2,25","OK",200))return 8;			//�����ı�ģʽ 	
	
	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",200))return 5;	//����TE�ַ���ΪUCS2

	sprintf((char*)cmd,"AT+CMGS=\"%s\"",msisdn);//�����ʽ��AT+CMGS="XXX"
	
	if(sim900a_send_cmd((u8*)cmd,">",200))return 6;	//���ö���Ϣ�ı�ģʽ���� 
	
	u3_printf("6211662F4E2D56FD4EBA");
//	u3_printf("Dbb is %d years old,and today's temperature is %.2fC,now is %2d:%d ,high is %.3fm,Good Night Dbb",27,34.5678,1,30,154.6789);
//	u3_printf("%s","Dear user,This is a test message from atk-sim900a module.");//���Ͷ������ݵ�GSMģ�� 
	
	if(sim900a_send_cmd((u8*)0X1A,"+CMGS:",1000)) return 7;//���ͽ�����
	
  return 0;
}
*/


