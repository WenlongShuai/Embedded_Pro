#include "usart6.h"
#include <string.h>
#include <stdarg.h>
#include "stdio.h"

unsigned char ucRxData6[100];
unsigned char ucRxFinish6=0;




void usart6_init(u32 bound)
{  
//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//ʹ��USART2��GPIOAʱ��
 	USART_DeInit(USART6);  //��λ����2
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);  //GPIOB11����ΪUSART3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);  //GPIOB10����ΪUSART3
	
	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart2 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART6, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART6, ENABLE);                    //ʹ�ܴ��� 
}


void USART6_IRQHandler(void)                	//����1�жϷ������
{
	static u8 ucCnt6=0;
	unsigned char temp=0;

	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  
		{
		  temp=USART_ReceiveData(USART6);
			ucRxData6[ucCnt6++]=temp;
			if(temp=='f') 
			{ 
				ucRxFinish6=1;
			  ucCnt6=0;
			}				
		}
	  USART_ClearITPendingBit(USART6, USART_IT_RXNE);
}   




__align(8) u8 USART6_TX_BUF[400]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

void u6_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART6_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);                     //�˴η������ݵĳ���
	for(j=0;j<i;j++)                                          //ѭ����������
	{
	  while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);//�ȴ��ϴδ������ 
		USART_SendData(USART6,(uint8_t)USART6_TX_BUF[j]); 	  //�������ݵ�����3 
	}
	
}






