#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include "usartx.h"
#include "SysTick.h"

uint8_t set_Bluetooth_Param(uint8_t type, uint8_t *paramStr)
{
	memset(Bluetooth_USART_RX_BUF,0,BLUETOOTH_USART_REC_LEN);
	switch(type)
	{
		case 1:   //版本号
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+VERSION\r\n");
			break;
		case 2:		//软复位
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+RESET\r\n");
			break;
		case 3:   //断开连接（连接状态下有效）
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DISC\r\n");
			break;
		case 4:  	//查询模块MAC地址
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+LADDR\r\n");
			break;
		case 5:  	//连接密码设置与查询
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+PIN\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+PIN%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 6: 	//波特率设置与查询
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+BAUD\r\n");
			else
			{
				uint8_t str[128] = {0};
				if(!strcmp((char *)paramStr, (const char *)"9600"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD4\r\n");
				}
				else if(!strcmp((char *)paramStr, (char *)"19200"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD5\r\n");
				}
				else if(!strcmp((char *)paramStr, (char *)"38400"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD6\r\n");
				}
				else if(!strcmp((char *)paramStr, (char *)"57600"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD7\r\n");
				}
				else if(!strcmp((char *)paramStr, (char *)"115200"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD8\r\n");
				}
				else if(!strcmp((char *)paramStr, (char *)"128000"))
				{
					strcpy((char *)str, (const char *)"AT+BAUD9\r\n");
				}
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 7: 	//广播名设置与查询
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+NAME\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+NAME%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 8: 	//恢复出厂设置
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DEFAULT\r\n");
			break;	
		case 9: 	//串口状态输出使能
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+ENLOG\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+ENLOG%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		default:
			break;
	}
	
	Delay_ms(10);
	if(Bluetooth_USART_RX_BUF[0] == '\0') 
	{
		return 1; //发送失败
	}
	
	printf("%s\r\n",Bluetooth_USART_RX_BUF);
	return 0;
}


