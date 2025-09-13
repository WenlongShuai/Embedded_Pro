#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "FreeRTOS.h"
#include "queue.h"

// 声明全局消息队列句柄
QueueHandle_t xAppSerialCmdQueue = NULL;
	
u8 Bluetooth_USART_RX_BUF[BLUETOOTH_USART_REC_LEN] = {0};     //接收缓冲,最大USART_REC_LEN个字节.
//接收数据计数
u16 Bluetooth_USART_RX_STA_COUNT = 0;

void Bluetooth_USART_GPIO_Init(uint32_t bound)
{
	// GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLUETOOTH_USART_RCC_GPIO | BLUETOOTH_STATE_RCC_GPIO, ENABLE); // 使能GPIO时钟
	RCC_APB1PeriphClockCmd(BLUETOOTH_USART_RCC,ENABLE);// 使能USART时钟
 
	// 串口对应引脚复用映射
	GPIO_PinAFConfig(BLUETOOTH_USART_RX_GPIO, BLUETOOTH_RX_PinSourcex, BLUETOOTH_RX_AF); //复用为USART
	GPIO_PinAFConfig(BLUETOOTH_USART_TX_GPIO, BLUETOOTH_TX_PinSourcex, BLUETOOTH_TX_AF); //复用为USART
	
	// USART端口配置
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BLUETOOTH_USART_RX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_TX_PIN;
	GPIO_Init(BLUETOOTH_USART_TX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_STATE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BLUETOOTH_STATE_GPIO, &GPIO_InitStructure);
	
  //USART初始化设置
	USART_InitStructure.USART_BaudRate = bound;// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收发模式
  USART_Init(BLUETOOTH_USARTx, &USART_InitStructure);
	
  USART_Cmd(BLUETOOTH_USARTx, ENABLE);  // 使能串口 
		
	USART_ITConfig(BLUETOOTH_USARTx, USART_IT_RXNE, ENABLE);// 开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = BLUETOOTH_USART_IRQn;// 串口中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;// 抢占优先级9
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
}

void Bluetooth_USART_Send_Str(uint8_t *str)
{
	while(*str != '\0')
	{
		while((BLUETOOTH_USARTx->SR&0X40)==0);// 循环发送,直到发送完毕   
		BLUETOOTH_USARTx->DR = *str;
		str++;
	}
}

uint8_t set_Bluetooth_Param(uint8_t type, uint8_t *paramStr)
{
	memset(Bluetooth_USART_RX_BUF,0,BLUETOOTH_USART_REC_LEN);

	switch(type)
	{
		case 1:   // 版本号
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+VERSION\r\n");
			break;
		case 2:		// 软复位
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+RESET\r\n");
			break;
		case 3:   // 断开连接（连接状态下有效）
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DISC\r\n");
			break;
		case 4:  	// 查询模块MAC地址
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+LADDR\r\n");
			break;
		case 5:  	// 连接密码设置与查询
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+PIN\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+PIN%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 6: 	// 波特率设置与查询
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
		case 7: 	// 广播名设置与查询
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+NAME\r\n");
			else
			{
				uint8_t str[128] = {0};
				sprintf((char *)str, "AT+NAME%s\r\n", paramStr);
				Bluetooth_USART_Send_Str((uint8_t *)str);
			}
			break;
		case 8: 	// 恢复出厂设置
			if(paramStr == NULL)
				Bluetooth_USART_Send_Str((uint8_t *)"AT+DEFAULT\r\n");
			break;	
		case 9: 	// 串口状态输出使能
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
	//0x0a:换行符   0x0d:回车符
	u8 Res;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(USART_GetITStatus(BLUETOOTH_USARTx, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(BLUETOOTH_USARTx);//(USART3->DR);	//读取接收到的数据
		Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] = Res;
		if(Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT-1] == 0x0D && Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] == 0x0A)
		{
			Bluetooth_USART_RX_STA_COUNT = 0;
			
			// 命令接收完成，发送到队列
      xQueueSendFromISR(xAppSerialCmdQueue, Bluetooth_USART_RX_BUF, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			
			memset(Bluetooth_USART_RX_BUF,0,BLUETOOTH_USART_REC_LEN);
		}
		else
			Bluetooth_USART_RX_STA_COUNT++;
		 
		USART_ClearITPendingBit(BLUETOOTH_USARTx, USART_IT_RXNE);
	}
}

