#include "yaw.h" 
#include "usart2.h"
#include "delay.h"
#include <string.h>

void QMC_5883L_send_cmd(char *cmd)
{
	u8 res=0; 
	if((u32)cmd<=0XFF)
	{
		while((USART2->SR&0X40)==0);//等待上一次数据发送完成  
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);  //发送命令
}


char*QMC_5883L_AT_CMD_TBL[8]={"AT","AT+UART=1","AT+INIT","AT+PRATE=150",\
															"AT+CALI=1","AT+CALI=0","AT+CALI=2","AT+FILT=500"};	
void QMC_5883L_Init(void)
{
	u8 i=0;
	for(i=0;i<8;i++)
	{
		QMC_5883L_send_cmd(QMC_5883L_AT_CMD_TBL[i]);
		delay_ms(100);
	}
}


u8 CommaFind(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

u32 a_x_fun(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

int YAW_str_to_num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		//if( (*p=='\0')||(*p=='\r')||(*p=='\n') )break;//遇到结束了
		if( *p=='\r')break;//遇到结束了
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
		ires+=a_x_fun(10,ilen-1-i)*(buf[i]-'0');
	}
	
	
	if(flen>2) flen=2;
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=a_x_fun(10,flen-1-i)*(buf[ilen+1+i]-'0');
	}
	if(flen<2) 
	{
		fres*=10;
		flen=2;
	}
	
	*dx=flen;	 		//小数点位数	
	
	res=ires*a_x_fun(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	

void YAW_Analysis(yaw_msg* yaw_var,u8 *buf_var)
{
	u8 *p1;
	u8 dx;	
	u8 posx; 
	int temp;
	p1=(u8*)strstr((const char *)buf_var,"Magx");
	posx=CommaFind(p1,3);
	temp=YAW_str_to_num(p1+posx+4,&dx);	
	yaw_var->yaw=(float)temp/a_x_fun(10,dx);
}




#if 0
void QMC_5883L_Init()
{
	QMC_5883L_send_cmd("AT");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+UART=1");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+INIT");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+PRATE=150");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+CALI=1");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+CALI=0");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+CALI=2");
	delay_ms(100);
	QMC_5883L_send_cmd("AT+FILT=500");	
	delay_ms(100);	
}
#endif