#include "usart6.h"
#include <string.h>
#include <stdarg.h>
#include "stdio.h"

unsigned char ucRxData6[100];
unsigned char ucRxFinish6=0;




void usart6_init(u32 bound)
{  
//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能USART2，GPIOA时钟
 	USART_DeInit(USART6);  //复位串口2
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);  //GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);  //GPIOB10复用为USART3
	
	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PA9
   
    //USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PA10

   //Usart2 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART6, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART6, ENABLE);                    //使能串口 
}


void USART6_IRQHandler(void)                	//串口1中断服务程序
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




__align(8) u8 USART6_TX_BUF[400]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节

void u6_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART6_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);                     //此次发送数据的长度
	for(j=0;j<i;j++)                                          //循环发送数据
	{
	  while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);//等待上次传输完成 
		USART_SendData(USART6,(uint8_t)USART6_TX_BUF[j]); 	  //发送数据到串口3 
	}
	
}






