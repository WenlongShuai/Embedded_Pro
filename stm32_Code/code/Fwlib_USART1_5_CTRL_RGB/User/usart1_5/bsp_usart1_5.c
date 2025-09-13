#include "bsp_usart1_5.h"

uint8_t receiveData = 0;

static void NVIC_USART_Config()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(USART_NVIC_PRIORITYGROUP);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART_NVIC_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = USART_NVIC_PREEPRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = USART_NVIC_SUBPRIORITY;
	
	NVIC_Init(&NVIC_InitStruct);
}

static void Usart_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	USART_GPIO_RCC_PeriphClockCmd(UASRT_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = USART_TX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = USART_RX_GPIO_PIN;
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStruct);
}

static void Usart_Config()
{
	USART_InitTypeDef USART_InitStruct;
	
	USART_RCC_PeriphClockCmd(USART_CLK, ENABLE);
	
	USART_InitStruct.USART_BaudRate = USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL;
	USART_InitStruct.USART_Mode = USART_MODE;
	USART_InitStruct.USART_Parity = USART_PARITY;
	USART_InitStruct.USART_StopBits = USART_STOPBITS;
	USART_InitStruct.USART_WordLength = USART_WORDLENGTH;
	
	USART_Init(USARTx, &USART_InitStruct);
	
	USART_Cmd(USARTx, ENABLE);
	
	NVIC_USART_Config();
	
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
	
}

void USART_GPIO_Init()
{
	Usart_GPIO_Config();
	Usart_Config();
}

void Usart_SendByteData(USART_TypeDef* USART, uint8_t sendData)
{
	USART_SendData(USART, sendData);
	while(USART_GetFlagStatus(USART, USART_FLAG_TXE) != SET);
}

void Usart_SendHalfWordData(USART_TypeDef* USART, uint16_t sendData)
{
	uint8_t temp_h = 0x00;
	uint8_t temp_l = 0x00;
	
	temp_h = (sendData & 0xff00) >> 8;
	temp_l = sendData & 0x00ff;
	
	Usart_SendByteData(USART, temp_h);
	Usart_SendByteData(USART, temp_l);
}

void Usart_SendArrayData(USART_TypeDef* USART, uint8_t *sendData, uint8_t arrayLen)
{
	uint8_t i = 0;
	for(i = 0;i < arrayLen;i++)
	{
		Usart_SendByteData(USART, *(sendData+i));
	}
	while(USART_GetFlagStatus(USART, USART_FLAG_TC) != SET);
}

void Usart_SendStrData(USART_TypeDef* USART, uint8_t *sendData)
{
	while(*sendData != '\0')
	{
		Usart_SendByteData(USART, *sendData);
		sendData++;
	}
	while(USART_GetFlagStatus(USART, USART_FLAG_TC) != SET);
}
