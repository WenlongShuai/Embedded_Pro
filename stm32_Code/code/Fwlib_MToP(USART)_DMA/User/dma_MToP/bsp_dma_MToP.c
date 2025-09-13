#include "bsp_dma_MToP.h"

uint8_t memoryData[BUFFER_SIZE] = {0};

static void USART_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开PA9、PA10的时钟
	RCC_APB2PeriphClockCmd(USART_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = USART_GPIO_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_Init(USART_GPIO_TX_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = USART_GPIO_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(USART_GPIO_RX_PORT, &GPIO_InitStruct);
}

void USART_DMA_Config()
{
	USART_InitTypeDef USART_InitStruct;
	//打开USART1的时钟
	RCC_APB2PeriphClockCmd(USART_CLK, ENABLE);
	
	USART_InitStruct.USART_BaudRate = USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HARDWAREFLOWCTRL;
	USART_InitStruct.USART_Mode = USART_MODE;
	USART_InitStruct.USART_Parity = USART_PARITY;
	USART_InitStruct.USART_StopBits = USART_STOPBITS;
	USART_InitStruct.USART_WordLength = USART_WORDLENGTH;
	
	USART_Init(DEBUG_USART, &USART_InitStruct);
	
	
	USART_Cmd(DEBUG_USART, ENABLE);
	
	USART_GPIO_Config();
}

/* 配置DMA的工作模式为 M--->P */
void MToP_DMA_Config()
{
	DMA_InitTypeDef MToM_DMA_InitStruct;
	
	RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
	
	/* DMA的数据从哪里来，到哪里去 */
 	MToM_DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(USART1_BASE+0x04);
	MToM_DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)memoryData;
	MToM_DMA_InitStruct.DMA_DIR = DMA_PERIPHERAL_DIR;
	
	/* DMA一次性传多少数据，数据的宽度 */
	MToM_DMA_InitStruct.DMA_BufferSize = DMA_BUFFERSIZE;
	MToM_DMA_InitStruct.DMA_PeripheralInc = DMA_PERIPHERAL_INC;
	MToM_DMA_InitStruct.DMA_PeripheralDataSize = DMA_PERIPHERAL_DATASIZE;
	MToM_DMA_InitStruct.DMA_MemoryInc = DMA_MEMORY_INC;
	MToM_DMA_InitStruct.DMA_MemoryDataSize = DMA_MEMORY_DATASIZE;
	
	/* DMA 数据什么时候传输结束 */
	MToM_DMA_InitStruct.DMA_Mode = DMA_MODE;  //正常传输完成结束，不需要循环传输
	
	MToM_DMA_InitStruct.DMA_M2M = DMA_MToM;  //DMA是否为MToM
	MToM_DMA_InitStruct.DMA_Priority = DMA_PRIORITY;  //DMA的优先级
	
	DMA_Init(DMA_CHANNEL, &MToM_DMA_InitStruct);
	
	DMA_ClearFlag(DMA_TC_FLAG);  //为了保证传输完成不被干扰，所以在初始化时就清除完成标志位
	
	DMA_Cmd(DMA_CHANNEL, ENABLE);
}

void Usart_SendByteData(USART_TypeDef* USART, uint8_t sendData)
{
	USART_SendData(USART, sendData);
	while(USART_GetFlagStatus(USART, USART_FLAG_TXE) != SET);
}


