#include "jy901p_usart.h"
#include "jy901p.h"


/**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
static void JY901P_USART_Config(unsigned int uiBaud)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	//USART Initialization Settings ��ʼ������
	USART_InitStructure.USART_BaudRate = uiBaud;  // ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // һ��ֹͣ
	USART_InitStructure.USART_Parity = USART_Parity_No; // ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �շ�ģʽ
	USART_Init(USART6, &USART_InitStructure);
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
	USART_Cmd(USART6, ENABLE);                     // ʹ�ܴ���
}

/**
  * @brief UART MSP ��ʼ�� 
  * @param huart: UART handle
  * @retval ��
  */
static void JY901P_GPIO_Config(void)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	 // ʹ��GPIOʱ��

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;     // RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  // TX
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void JY901P_USART_Init(unsigned int uiBaud)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	JY901P_USART_Config(uiBaud);
	JY901P_GPIO_Config();

	// UsartNVIC����
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  // IRQͨ��ʹ��	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// ����ָ���Ĳ�����ʼ��VIC�Ĵ���		
	NVIC_Init(&NVIC_InitStructure);	
}

// ��ս��ջ���
void JY901P_FlushRxBuffer(void)
{
//  UART_RxPtr = 0;
//  UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  �����ַ� **********************/
void JY901P_SendByte(uint8_t str)
{
	USART_SendData(USART6, str);
}

/*****************  �����ַ��� **********************/
void JY901P_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// �ȴ����ͻ�����Ϊ��
			while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
			
			// ��������
			USART_SendData(USART6, str[i]);
	}
	
	// �ȴ��������
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
}

void USART6_IRQHandler()
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		uint8_t res = USART_ReceiveData(USART6);//(USART6->DR);	//��ȡ���յ�������
		WitSerialDataIn(res);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	}
}
