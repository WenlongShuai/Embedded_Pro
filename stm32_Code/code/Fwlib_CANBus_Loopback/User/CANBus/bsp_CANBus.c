#include "bsp_CANBus.h"

CanTxMsg can_Tx_Data = {0};
CanRxMsg can_Rx_Data = {0};

static void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = CAN_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_Init(&NVIC_InitStruct);
	
}

/* 如果stm32的bxCAN有外接 CAN收发器，就要通过下面的函数初始化CAN收发器相应的GPIO */
/* 如果没有CAN收发器就不要使用该函数 */
static void GPIO_Config()
{
	/* CAN收发器 GPIO 相关的初始化 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	CAN_GPIO_CLK_FUNC(CAN_GPIO_CLK, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = CAN_TX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = CAN_RX_GPIO_PIN;
	GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStruct);
}


static void CAN_Config(void)
{
	CAN_InitTypeDef CAN_InitStruct;   //CAN 的基本初始化结构体
	CAN_FilterInitTypeDef CAN_FilterInitStruct;  //CAN 的过滤器初始化结构体
	
	//打开CAN外设的时钟
	CAN_CLK_FUNC(CAN_CLK, ENABLE);
	CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack;  //设置CAN的测试模式，此处设置为环回模式，因为开发板没有外接CAN收发器
	CAN_InitStruct.CAN_ABOM = ENABLE;  //自动总线关闭管理设置
	CAN_InitStruct.CAN_AWUM = ENABLE;  //自动唤醒设置
	CAN_InitStruct.CAN_NART = DISABLE;  //非自动重传模式设置
	CAN_InitStruct.CAN_Prescaler = 4;
	CAN_InitStruct.CAN_RFLM = ENABLE;   //接收FIFO锁定模式设置
	CAN_InitStruct.CAN_TTCM = DISABLE;   //时间触发通信模式设置
	CAN_InitStruct.CAN_TXFP = DISABLE;   //发送FIFO优先级设置
	CAN_InitStruct.CAN_BS1 = CAN_BS1_5tq;  //(4+1) 
	CAN_InitStruct.CAN_BS2 = CAN_BS2_3tq;	 //(2+1)
	CAN_InitStruct.CAN_SJW = CAN_SJW_2tq;  //(1+1)
	
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask; //标识符屏蔽  //CAN_FilterMode_IdList;  //标识符列表
	CAN_FilterInitStruct.CAN_FilterNumber = 0;  //筛选器选择0~13组（每组2个32位寄存器）
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStruct.CAN_FilterIdHigh = (((CAN_FILTER_PASSID << 3)|CAN_Id_Extended|CAN_RTR_DATA)&0xFFFF0000) >> 16;
	CAN_FilterInitStruct.CAN_FilterIdLow = ((CAN_FILTER_PASSID << 3)|CAN_Id_Extended|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0xFFFF; //标识符扩展全部匹配
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0xFFFF;
	
	CAN_FilterInit(&CAN_FilterInitStruct);
	
	CAN_Init(CAN_x, &CAN_InitStruct);
	
	CAN_ITConfig(CAN_x, CAN_IT_FMP0, ENABLE);
	
	CAN_ClearITPendingBit(CAN_x, CAN_IT_FMP0);
}


void CANx_Init(void)
{
	NVIC_Config();
	GPIO_Config();
	CAN_Config();
}

