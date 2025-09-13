#include "pid_usart.h"
#include "protocol.h"

//串口接收数组
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
//串口接收数组指针
unsigned char UART_RxPtr;
/* 命令接收完成 */
uint8_t receive_cmd = 0;

/**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
static void PID_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	//USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = 115200; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_Init(UART5, &USART_InitStructure);      //初始化串口5
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //开启串口接受中断
	USART_Cmd(UART5, ENABLE);                     //使能串口5
}

/**
  * @brief UART MSP 初始化 
  * @param huart: UART handle
  * @retval 无
  */
static void PID_GPIO_Config(void)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     // RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // TX
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void PID_USART_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	PID_USART_Config();
	PID_GPIO_Config();

	// UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	// 抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  // IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// 根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);	
}

// 清空接收缓冲
void uart_FlushRxBuffer(void)
{
  UART_RxPtr = 0;
  UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  发送字符 **********************/
void Usart_SendByte(uint8_t str)
{
	USART_SendData(UART5, str);
}

/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// 等待发送缓冲区为空
			while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
			
			// 发送数据
			USART_SendData(UART5, str[i]);
	}
	
	// 等待发送完成
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
}

void UART5_IRQHandler()
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		uint8_t res = USART_ReceiveData(UART5);//(USART1->DR);	//读取接收到的数据
		protocol_data_recv(&res, 1);
	}
}
