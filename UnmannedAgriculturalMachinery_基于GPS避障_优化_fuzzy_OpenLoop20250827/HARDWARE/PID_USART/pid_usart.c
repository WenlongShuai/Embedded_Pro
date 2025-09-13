#include "pid_usart.h"
#include "protocol.h"

//���ڽ�������
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
//���ڽ�������ָ��
unsigned char UART_RxPtr;
/* ���������� */
uint8_t receive_cmd = 0;

/**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
static void PID_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	//USART Initialization Settings ��ʼ������
	USART_InitStructure.USART_BaudRate = 115200; //Port rate //���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //һ��ֹͣ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure);      //��ʼ������5
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //�������ڽ����ж�
	USART_Cmd(UART5, ENABLE);                     //ʹ�ܴ���5
}

/**
  * @brief UART MSP ��ʼ�� 
  * @param huart: UART handle
  * @retval ��
  */
static void PID_GPIO_Config(void)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //ʹ��GPIOʱ��

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     // RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // TX
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void PID_USART_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	PID_USART_Config();
	PID_GPIO_Config();

	// UsartNVIC����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	// ��ռ���ȼ�6
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	// �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  // IRQͨ��ʹ��	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// ����ָ���Ĳ�����ʼ��VIC�Ĵ���		
	NVIC_Init(&NVIC_InitStructure);	
}

// ��ս��ջ���
void uart_FlushRxBuffer(void)
{
  UART_RxPtr = 0;
  UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  �����ַ� **********************/
void Usart_SendByte(uint8_t str)
{
	USART_SendData(UART5, str);
}

/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// �ȴ����ͻ�����Ϊ��
			while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
			
			// ��������
			USART_SendData(UART5, str[i]);
	}
	
	// �ȴ��������
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
}

void UART5_IRQHandler()
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		uint8_t res = USART_ReceiveData(UART5);//(USART1->DR);	//��ȡ���յ�������
		protocol_data_recv(&res, 1);
	}
}
