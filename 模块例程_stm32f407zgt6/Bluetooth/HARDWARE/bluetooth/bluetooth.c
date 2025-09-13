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
		case 1:   //�汾��
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+VERSION\r\n");
			break;
		case 2:		//��λ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+RESET\r\n");
			break;
		case 3:   //�Ͽ����ӣ�����״̬����Ч��
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DISC\r\n");
			break;
		case 4:  	//��ѯģ��MAC��ַ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+LADDR\r\n");
			break;
		case 5:  	//���������������ѯ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+PIN\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+PIN%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 6: 	//�������������ѯ
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
		case 7: 	//�㲥���������ѯ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+NAME\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+NAME%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 8: 	//�ָ���������
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DEFAULT\r\n");
			break;	
		case 9: 	//����״̬���ʹ��
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
		return 1; //����ʧ��
	}
	
	printf("%s\r\n",Bluetooth_USART_RX_BUF);
	return 0;
}


