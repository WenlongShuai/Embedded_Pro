#include "usartx.h"
#include <stdlib.h>
#include <string.h>

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 Bluetooth_USART_RX_BUF[BLUETOOTH_USART_REC_LEN] = {0};     //���ջ���,���USART_REC_LEN���ֽ�.
//�������ݼ���
u16 Bluetooth_USART_RX_STA_COUNT = 0;

void Bluetooth_USART_GPIO_Init(uint32_t bound)
{
	 //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLUETOOTH_USART_RCC_GPIO,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(BLUETOOTH_USART_RCC,ENABLE);//ʹ��USART3ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(BLUETOOTH_USART_RX_GPIO, BLUETOOTH_RX_PinSourcex, BLUETOOTH_RX_AF); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(BLUETOOTH_USART_TX_GPIO, BLUETOOTH_TX_PinSourcex, BLUETOOTH_TX_AF); //GPIOB11����ΪUSART3
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_RX_PIN; //GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(BLUETOOTH_USART_RX_GPIO,&GPIO_InitStructure); //��ʼ��PB11
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_TX_PIN; //GPIOB10
	GPIO_Init(BLUETOOTH_USART_TX_GPIO,&GPIO_InitStructure); //��ʼ��PB10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(BLUETOOTH_USARTx, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(BLUETOOTH_USARTx, ENABLE);  //ʹ�ܴ���3 
		
	USART_ITConfig(BLUETOOTH_USARTx, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
  NVIC_InitStructure.NVIC_IRQChannel = BLUETOOTH_USART_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

void Bluetooth_USART_Send_Str(uint8_t *str)
{
	while(*str != '\0')
	{
		while((BLUETOOTH_USARTx->SR&0X40)==0);//ѭ������,ֱ���������   
		BLUETOOTH_USARTx->DR = *str;
		str++;
	}
}

void USART3_IRQHandler()
{
	//0x0a:���з�   0x0d:�س���
	u8 Res;
	if(USART_GetITStatus(BLUETOOTH_USARTx, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(BLUETOOTH_USARTx);//(USART3->DR);	//��ȡ���յ�������
		Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] = Res;
		Bluetooth_USART_RX_STA_COUNT++;
		if(Res == 0x0d)
		{
			Bluetooth_USART_RX_STA_COUNT = 0;
		}
		USART_ClearITPendingBit(BLUETOOTH_USARTx, USART_IT_RXNE);
	}
}

