//此文件仅需关注get_mode，其余为遥控相关，与比赛任务无关
#include "headfile.h"
#include "remote.h"
#include "pwm.h"
#include "user.h"
uint8_t remote_buf[40];
int test1,test2;
uint8 uart7_get_buffer;
REMOTE_DATA_ST remote_data;
void Remote_Data_Analysis(uint8_t *buf_data)
{
	test2++;
	remote_data.forword_speed = (buf_data[5]<<8) | (buf_data[6]);
	remote_data.turn_pwm = (buf_data[7]<<8) | (buf_data[8]);
	remote_data.mode_pwm = (buf_data[9]<<8) | (buf_data[10]);
//	tft_show_int(remote_data.forword_speed,0,0);
	printf("%d %d %d\r\n",remote_data.forword_speed,remote_data.turn_pwm,remote_data.mode_pwm);
}
void Remote_Byte_Get(uint8_t bytedata)
{	
//	printf("zvD");
//	uart_putchar(UART_1,bytedata);
	test1++;
	static uint8_t len = 0,rec_sta;
	uint8_t check_val=0;
	remote_buf[rec_sta] = bytedata;
	if(rec_sta==0)
	{
		if(bytedata==0xaa)
		{
			rec_sta++;
		}
		else
		{
			rec_sta=0;
		}
	}
	else if(rec_sta==1)
	{
		if(bytedata==0x29)
		{
			rec_sta++;
		}	
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==2)
	{
		if(bytedata==0x05)
		{
			rec_sta++;
		}
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==3)
	{
		if(bytedata==0x42)
		{
			rec_sta++;
		}
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==4)
	{
		//
		len = bytedata;
		if(len<40)
		{
			rec_sta++;
		}		
		else
		{
			rec_sta=0;
		}
	}
	else if(rec_sta==(len+5))
	{
		//
		for(uint8_t i=0;i<len+5;i++)
		{
			check_val += remote_buf[i];
		}
		//
		if(1)
		{
			//解析成功
			Remote_Data_Analysis(remote_buf);
			//
			rec_sta=0;
		}
		else
		{
			rec_sta=0;
//			uart_putchar(UART_1,check_val);
//			uart_putchar(UART_1,bytedata);
		}		
	}
	else
	{
		//	
		rec_sta++;
	}
	
}
void remote_hander(void){
	uart_getchar(UART_7, &uart7_get_buffer);
	Remote_Byte_Get(uart7_get_buffer);
}

int get_mode(void){
	return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9);
}
