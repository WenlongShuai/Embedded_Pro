#include "sim.h" 
#include "delay.h"	   
#include "usart.h"
#include "usart2.h"
#include "string.h" 
#include "press.h"
#include "gps.h" 

extern long Pa;
extern nmea_msg gpsx;

//ATK-SIM800C 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码
//SIM800C发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)  //接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向SIM800C发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART2->SR&0X40)==0);//等待上一次数据发送完成  
		USART2->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);  //发送命令
	
	if(ack&&waittime)		        //需要等待应答
	{
		while(--waittime)	        //等待倒计时
		{ 
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))break;//得到有效数据 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}



u8 sim900a_check_status(void)
{
	if(sim900a_send_cmd("AT","OK",200)) return 1;//发送AT指令，判断返回是否是OK。
    if(sim900a_send_cmd("AT+CPIN?","OK",200)) return 2;// 检查SIM卡失败。。。没有插入SIM卡。	
    if(sim900a_send_cmd("AT+CGMI","OK",200))return 3;//查询不到运营商；	
	return 0;	
}



u8 si900a_sms_test(u8* msisdn)
{
	u8 status=0;
	char cmd[20];
	
	status=sim900a_check_status();
	if(status) return status; 
	
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 4;			//设置文本模式 
	
	if(sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200))return 5;	//设置TE字符集为UCS2

	sprintf((char*)cmd,"AT+CMGS=\"%s\"",msisdn);//命令格式：AT+CMGS="XXX"
	
	if(sim900a_send_cmd((u8*)cmd,">",200))return 6;	//设置短消息文本模式参数 
	
	u2_printf("Current location information : %.3f%1c , %.3f%1c , altitude is %.1fm , and underwater pressure is %.3fKPa . ",gpsx.longitude/100000.0,gpsx.ewhemi,gpsx.latitude/100000.0,gpsx.nshemi,gpsx.altitude/10.0,Pa/1000.0);
	   printf("\r\n当前位置信息：%.3f%1c，%.3f%1c，海拔高度%.1fm，水压%.3fKPa。\r\n\r\n",gpsx.longitude/100000.0,gpsx.ewhemi,gpsx.latitude/100000.0,gpsx.nshemi,gpsx.altitude/10.0,Pa/1000.0);
	//u2_printf("Dbb is %d years old,and today's temperature is %.2fC,now is %2d:%d ,high is %.3fm,Good Night Dbb",27,34.5678,1,30,154.6789);
	
	if(sim900a_send_cmd((u8*)0X1A,"+CMGS:",1000)) return 7;//发送结束符
	
  return 0;
}



/*
u8 si900a_sms_test(u8* msisdn)
{
	u8 status=0;
	char cmd[20];
	
	status=sim900a_check_status();
	if(status) return status; 
	
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 4;			//设置文本模式
	
	if(sim900a_send_cmd("AT+CSMP=17,167,2,25","OK",200))return 8;			//设置文本模式 	
	
	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",200))return 5;	//设置TE字符集为UCS2

	sprintf((char*)cmd,"AT+CMGS=\"%s\"",msisdn);//命令格式：AT+CMGS="XXX"
	
	if(sim900a_send_cmd((u8*)cmd,">",200))return 6;	//设置短消息文本模式参数 
	
	u3_printf("6211662F4E2D56FD4EBA");
//	u3_printf("Dbb is %d years old,and today's temperature is %.2fC,now is %2d:%d ,high is %.3fm,Good Night Dbb",27,34.5678,1,30,154.6789);
//	u3_printf("%s","Dear user,This is a test message from atk-sim900a module.");//发送短信内容到GSM模块 
	
	if(sim900a_send_cmd((u8*)0X1A,"+CMGS:",1000)) return 7;//发送结束符
	
  return 0;
}
*/


