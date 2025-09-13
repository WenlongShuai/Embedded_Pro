#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "FreeRTOS.h"
#include "queue.h"

// ����ȫ����Ϣ���о��
QueueHandle_t xAppSerialCmdQueue = NULL;
	
u8 Bluetooth_USART_RX_BUF[BLUETOOTH_USART_REC_LEN] = {0};     //���ջ���,���USART_REC_LEN���ֽ�.
//�������ݼ���
u16 Bluetooth_USART_RX_STA_COUNT = 0;

void Bluetooth_USART_GPIO_Init(uint32_t bound)
{
	// GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLUETOOTH_USART_RCC_GPIO | BLUETOOTH_STATE_RCC_GPIO, ENABLE); // ʹ��GPIOʱ��
	RCC_APB1PeriphClockCmd(BLUETOOTH_USART_RCC,ENABLE);// ʹ��USARTʱ��
 
	// ���ڶ�Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(BLUETOOTH_USART_RX_GPIO, BLUETOOTH_RX_PinSourcex, BLUETOOTH_RX_AF); //����ΪUSART
	GPIO_PinAFConfig(BLUETOOTH_USART_TX_GPIO, BLUETOOTH_TX_PinSourcex, BLUETOOTH_TX_AF); //����ΪUSART
	
	// USART�˿�����
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BLUETOOTH_USART_RX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_TX_PIN;
	GPIO_Init(BLUETOOTH_USART_TX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BLUETOOTH_STATE_GPIO, &GPIO_InitStructure);
	
  //USART��ʼ������
	USART_InitStructure.USART_BaudRate = bound;// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// �շ�ģʽ
  USART_Init(BLUETOOTH_USARTx, &USART_InitStructure);
	
  USART_Cmd(BLUETOOTH_USARTx, ENABLE);  // ʹ�ܴ��� 
		
	USART_ITConfig(BLUETOOTH_USARTx, USART_IT_RXNE, ENABLE);// ��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = BLUETOOTH_USART_IRQn;// �����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;// ��ռ���ȼ�9
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
}

void Bluetooth_USART_Send_Str(uint8_t *str)
{
	while(*str != '\0')
	{
		while((BLUETOOTH_USARTx->SR&0X40)==0);// ѭ������,ֱ���������   
		BLUETOOTH_USARTx->DR = *str;
		str++;
	}
}

uint8_t set_Bluetooth_Param(uint8_t type, uint8_t *paramStr)
{
	memset(Bluetooth_USART_RX_BUF,0,BLUETOOTH_USART_REC_LEN);

	switch(type)
	{
		case 1:   // �汾��
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+VERSION\r\n");
			break;
		case 2:		// ��λ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+RESET\r\n");
			break;
		case 3:   // �Ͽ����ӣ�����״̬����Ч��
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DISC\r\n");
			break;
		case 4:  	// ��ѯģ��MAC��ַ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+LADDR\r\n");
			break;
		case 5:  	// ���������������ѯ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+PIN\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+PIN%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 6: 	// �������������ѯ
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
		case 7: 	// �㲥���������ѯ
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+NAME\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+NAME%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 8: 	// �ָ���������
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DEFAULT\r\n");
			break;	
		case 9: 	// ����״̬���ʹ��
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
	
	delay_ms(50);
		
	return 0;
}


void USART3_IRQHandler()
{
	//0x0a:���з�   0x0d:�س���
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(USART_GetITStatus(BLUETOOTH_USARTx, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(BLUETOOTH_USARTx);//(USART3->DR);	//��ȡ���յ�������
		Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] = Res;
		if(Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT-1] == 0x0D && Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] == 0x0A)
		{
			Bluetooth_USART_RX_STA_COUNT = 0;
			
			// ���������ɣ����͵�����
      xQueueSendFromISR(xAppSerialCmdQueue, Bluetooth_USART_RX_BUF, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			
			memset(Bluetooth_USART_RX_BUF,0,BLUETOOTH_USART_REC_LEN);
		}
		else
			Bluetooth_USART_RX_STA_COUNT++;
		 
		USART_ClearITPendingBit(BLUETOOTH_USARTx, USART_IT_RXNE);
	}
}

