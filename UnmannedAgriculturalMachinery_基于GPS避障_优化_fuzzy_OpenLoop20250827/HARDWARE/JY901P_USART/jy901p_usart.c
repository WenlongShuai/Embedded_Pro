#include "jy901p_usart.h"
#include "jy901p.h"


/**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
static void JY901P_USART_Config(unsigned int uiBaud)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	//USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = uiBaud;  // 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
	USART_Init(USART6, &USART_InitStructure);
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // 开启串口接受中断
	USART_Cmd(USART6, ENABLE);                     // 使能串口
}

/**
  * @brief UART MSP 初始化 
  * @param huart: UART handle
  * @retval 无
  */
static void JY901P_GPIO_Config(void)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	 // 使能GPIO时钟

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;     // RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  // TX
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void JY901P_USART_Init(unsigned int uiBaud)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	JY901P_USART_Config(uiBaud);
	JY901P_GPIO_Config();

	// UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// 子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  // IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// 根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);	
}

// 清空接收缓冲
void JY901P_FlushRxBuffer(void)
{
//  UART_RxPtr = 0;
//  UART_RxBuffer[UART_RxPtr] = 0;
}

/*****************  发送字符 **********************/
void JY901P_SendByte(uint8_t str)
{
	USART_SendData(USART6, str);
}

/*****************  发送字符串 **********************/
void JY901P_SendString(uint8_t *str, uint32_t len)
{	
	uint16_t i;
    
	for(i = 0; i < len; i++)
	{
			// 等待发送缓冲区为空
			while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
			
			// 发送数据
			USART_SendData(USART6, str[i]);
	}
	
	// 等待发送完成
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
}

void USART6_IRQHandler()
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		uint8_t res = USART_ReceiveData(USART6);//(USART6->DR);	//读取接收到的数据
		WitSerialDataIn(res);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	}
}
