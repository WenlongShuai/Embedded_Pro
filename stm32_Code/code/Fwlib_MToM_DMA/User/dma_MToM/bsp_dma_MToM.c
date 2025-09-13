#include "bsp_dma_MToM.h"

const uint32_t FLASH_Data[BUFFER_SIZE] = { 0x10000000,0x11000001,0x1200002,0x13000003,
																				0x14000004,0x15000005,0x16000006,0x17000007,
																				0x18000008,0x19000009,0x1A00000A,0x1B00000B,
																				0x1C00000C,0x1D00000D,0x1E00000E,0x1F00000F,
																				0x12000000,0x12000001,0x12000002,0x12000003,
																				0x12000004,0x12000005,0x12000006,0x12000007,
																				0x12000008,0x12000009,0x1200000A,0x1200000B,
																				0x1200000C,0x1200000D,0x1200000E,0x1200000F};

uint32_t SRAM_Data[BUFFER_SIZE] = {0};

static void NVIC_DMA_Config()
{
	NVIC_InitTypeDef NVIC_DMA_InitStruct;
	
	NVIC_PriorityGroupConfig(DMA_NVIC_PRIORITYGROUP);
	
	NVIC_DMA_InitStruct.NVIC_IRQChannel = DMA_NVIC_IRQ;
	NVIC_DMA_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_DMA_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_DMA_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_DMA_InitStruct);
}

/* 配置DMA的工作模式为 M--->M */
void MToM_DMA_Config()
{
	DMA_InitTypeDef MToM_DMA_InitStruct;
	
	RCC_AHBPeriphClockCmd(DMA_CLK, ENABLE);
	
	/* DMA的数据从哪里来，到哪里去 */
 	MToM_DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)FLASH_Data;
	MToM_DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SRAM_Data;
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
	//DMA_ClearITPendingBit(DMA1_IT_TC1);  //清除中断完成标志位
	
	DMA_Cmd(DMA_CHANNEL, ENABLE);
	
	/* 如果在传输完成之后，需要进行中断操作，就打开这两个函数 */
	//NVIC_DMA_Config();
	//DMA_ITConfig(DMA_CHANNEL, DMA_TC_IT, ENABLE);
}


uint8_t buffer_Compare(const uint32_t *buffer1, uint32_t *buffer2, uint16_t bufferLen)
{
	if(buffer1 == NULL || buffer2 == NULL)
	{
		return 0;
	}
	
	while(bufferLen--)
	{
		if(*buffer1 != *buffer2)
		{
			return 0;
		}
		buffer1++;
		buffer2++;
	}
	return 1;
}


