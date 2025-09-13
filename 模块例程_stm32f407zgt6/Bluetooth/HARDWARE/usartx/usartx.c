#include "usartx.h"
#include <stdlib.h>
#include <string.h>

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 Bluetooth_USART_RX_BUF[BLUETOOTH_USART_REC_LEN] = {0};     //接收缓冲,最大USART_REC_LEN个字节.
//接收数据计数
u16 Bluetooth_USART_RX_STA_COUNT = 0;

void Bluetooth_USART_GPIO_Init(uint32_t bound)
{
	 //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLUETOOTH_USART_RCC_GPIO,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(BLUETOOTH_USART_RCC,ENABLE);//使能USART3时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(BLUETOOTH_USART_RX_GPIO, BLUETOOTH_RX_PinSourcex, BLUETOOTH_RX_AF); //GPIOB10复用为USART3
	GPIO_PinAFConfig(BLUETOOTH_USART_TX_GPIO, BLUETOOTH_TX_PinSourcex, BLUETOOTH_TX_AF); //GPIOB11复用为USART3
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_RX_PIN; //GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(BLUETOOTH_USART_RX_GPIO,&GPIO_InitStructure); //初始化PB11
	
	GPIO_InitStructure.GPIO_Pin = BLUETOOTH_USART_TX_PIN; //GPIOB10
	GPIO_Init(BLUETOOTH_USART_TX_GPIO,&GPIO_InitStructure); //初始化PB10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(BLUETOOTH_USARTx, &USART_InitStructure); //初始化串口3
	
  USART_Cmd(BLUETOOTH_USARTx, ENABLE);  //使能串口3 
		
	USART_ITConfig(BLUETOOTH_USARTx, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  NVIC_InitStructure.NVIC_IRQChannel = BLUETOOTH_USART_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

void Bluetooth_USART_Send_Str(uint8_t *str)
{
	while(*str != '\0')
	{
		while((BLUETOOTH_USARTx->SR&0X40)==0);//循环发送,直到发送完毕   
		BLUETOOTH_USARTx->DR = *str;
		str++;
	}
}

void USART3_IRQHandler()
{
	//0x0a:换行符   0x0d:回车符
	u8 Res;
	if(USART_GetITStatus(BLUETOOTH_USARTx, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(BLUETOOTH_USARTx);//(USART3->DR);	//读取接收到的数据
		Bluetooth_USART_RX_BUF[Bluetooth_USART_RX_STA_COUNT] = Res;
		Bluetooth_USART_RX_STA_COUNT++;
		if(Res == 0x0d)
		{
			Bluetooth_USART_RX_STA_COUNT = 0;
		}
		USART_ClearITPendingBit(BLUETOOTH_USARTx, USART_IT_RXNE);
	}
}

