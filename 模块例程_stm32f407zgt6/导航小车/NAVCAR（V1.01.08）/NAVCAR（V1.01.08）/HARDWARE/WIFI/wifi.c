#include "wifi.h" 
#include "usart6.h"
#include "delay.h"
#include <string.h>
#include <stdarg.h>
#include "stdio.h"

extern unsigned char ucRxData6[100];

void ESP8266_send_cmd(char *cmd)
{
	u8 res=0; 
	if((u32)cmd<=0XFF)
	{
		while((USART6->SR&0X40)==0);//等待上一次数据发送完成  
		USART6->DR=(u32)cmd;
	}else u6_printf("%s\r\n",cmd);  //发送命令
}


char WIFI_Cmd_Buff[20]; 
void ESP8266_SendDate(char *date)
{
	u8 res=0; 
	u8 StrLength=0;
	StrLength=strlen(date);
	sprintf((char *)WIFI_Cmd_Buff,"AT+CIPSEND=0,%d",StrLength+2);
	u6_printf("%s\r\n",WIFI_Cmd_Buff);  //发送命令
	delay_ms(50);
	u6_printf("%s\r\n",date);  //发送数据
//	delay_ms(500);
//  StrLength=sizeof(date)-1;
//	sprintf((char *)WIFI_Cmd_Buff,"AT+CIPSEND=0,%d",StrLength);
//	u6_printf("%s\r\n",WIFI_Cmd_Buff);  //发送命令
//	ESP8266_send_cmd("AT+CIPSEND=0,30");
}


char*ESP8266_AT_CMD_TBL[6]={"AT+CWMODE=2","AT+RST","AT+CWSAP=\"MyWifi\",\"123654dbb\",1,4",\
	                           "AT+CIPMUX=1","AT+CIPSERVER=1,6050","AT+CIFSR"};	

void ESP8266_Init(void)
{
	u8 i=0;
	for(i=0;i<6;i++)
	{
		ESP8266_send_cmd(ESP8266_AT_CMD_TBL[i]);
		delay_ms(1000);
	}
}


u32 WIFI_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}


int WIFI_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==',')break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=WIFI_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=WIFI_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*WIFI_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	


void Get_Value_From_WIFI(unsigned char* ucFlag,double *JD,double *WD)
{
	unsigned char *p;
	int temp;
	u32 zs,xs;
	u8 d;	
	if(*ucFlag==1)
	{
		p=(unsigned char*)strstr((char*)ucRxData6,"JD:");
		p=p+3;
		temp=WIFI_Str2num(p,&d);
		zs=temp/WIFI_Pow(10,d);
		xs=temp%WIFI_Pow(10,d);
		*JD=zs+xs/100000.0;
		
		p=(unsigned char*)strstr((char*)ucRxData6,"WD:");
		p=p+3;
		temp=WIFI_Str2num(p,&d);
		zs=temp/WIFI_Pow(10,5);
		xs=temp%WIFI_Pow(10,5);
		*WD=zs+xs/100000.0;
		
		p=NULL;
		*ucFlag=0;		
	}
}
