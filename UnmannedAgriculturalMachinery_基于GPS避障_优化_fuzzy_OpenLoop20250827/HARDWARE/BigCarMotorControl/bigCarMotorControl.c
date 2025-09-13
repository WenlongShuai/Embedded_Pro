#include "bigCarMotorControl.h"
#include "fun.h"
#include <string.h>


extern float RC_Velocity;
extern float Move_X, Move_Y, Move_Z;
extern uint8_t Flag_Direction;

float bigCarMotorSpeedLeft;   // 大车左轮速度
float bigCarMotorSpeedRight;  // 大车右轮速度
SpeedUnion SpeedUnionArray[2] = {0};
rtkUnion rtkUnionArray[2] = {0};
uint8_t rtkUpdateFlag = 0;


// 事件标志组句柄
EventGroupHandle_t xEventGroup = NULL;

uint8_t bigCar_ReceiveDataBuf[BIGCAR_RECEIVE_DATA_LEN] = {0};  // 接收大车传过来的数据
uint8_t bufCount = 0;      // 接收数据计数

static void bigCarMotorControl_USART_GPIO_Init(uint32_t bound)
{
	// GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); // 使能USART时钟

	//串口对应引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4); // 复用为UART
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); // 复用为UART
	
	//UART_TX端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// UART_RX端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

  // UART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收发模式
  USART_Init(UART4, &USART_InitStructure); // 初始化串口
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	// 开启相关中断
	
	//UART4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;// 串口中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;// 抢占优先级10
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(UART4, ENABLE);  //使能串口
}

void bigCarMotorControlInit(void)
{
	bigCarMotorControl_USART_GPIO_Init(115200);
}

void UART4_Send_BufferData(const uint8_t *buffer, uint16_t len)
{
	for(int i = 0;i < len;i++)
	{		
		USART_SendData(UART4, buffer[i]);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	}
}

/**
@brief      设定大车左右电机的速度
@param      speed0 设定右电机的速度
            speed1 设定左电机的速度
@retval     None
@note       速度单位为dm/s，速度的正负可以表示方向
*/
bigCarSpeed bigCarMotor_Set_Speed(float speed0, float speed1)
{
	bigCarSpeed bigCarSpeedStruct;
	
	bigCarSpeedStruct.leftMotorSpeed = (int16_t)speed0 * (16800 / 5);
	
	bigCarSpeedStruct.rightMotorSpeed = (int16_t)speed1 * (16800 / 5);
	
	return bigCarSpeedStruct;
}


void getBigCarMotorSpeed(void)
{
	switch(Flag_Direction) // 处理方向控制命令
	{ 
		case 1:      Move_X = RC_Velocity/100.0f;  	 Move_Z = 0;         break;   //RC_Velocity/100;  RC_Velocity：串口发送过来的单位是mm，/100是将单位转换成dm
		case 2:      Move_X = 0;      				 		 Move_Z = -PI/2;   	 break;	 
		case 3:      Move_X = -RC_Velocity/100.0f;  	 Move_Z = 0;         break;	 
		case 4:      Move_X = 0;     	 			 	 		 Move_Z = +PI/2;     break;
		default:     Move_X = 0;               		 Move_Z = 0;         break;
	}

	if(Move_X<0) Move_Z = -Move_Z; // 差速控制原理系列需要此处理
	Move_Z = Move_Z*(RC_Velocity/100.0f);
	
	// 履带车运动学逆解
	bigCarMotorSpeedLeft = Move_X - Move_Z / 2.0f;     // 计算出左轮的目标速度
	bigCarMotorSpeedRight = Move_X + Move_Z / 2.0f;    // 计算出右轮的目标速度
	
	// 车轮(电机)目标速度限幅
	bigCarMotorSpeedLeft = target_limit_float( bigCarMotorSpeedLeft, -5, 5); 
	bigCarMotorSpeedRight = target_limit_float( bigCarMotorSpeedRight, -5, 5);
}

// 发送给上位机的数据打包
void bigCarMotorDataPack(uint8_t type, SpeedUnion *SpeedUnionParam, rtkUnion *rtkUnionParam, uint8_t *sendDataBuf)
{
	assert_param(SpeedUnionParam);
	assert_param(sendDataBuf);
	
	uint8_t frameHeader = 0x55;
	uint8_t frameTail = 0xaa;
	uint8_t funCode = 0;
	uint8_t buffSize = 0;
	
	buffSize = type ? 7 : 15;
	uint8_t buff[buffSize];
	
	switch(type)
	{
		case 0:
			funCode = 0xf2;
			buff[0] = frameHeader;
			buff[1] = funCode;
			buff[buffSize-1] = frameTail;
			memcpy(buff+2, SpeedUnionParam, 4);
			memcpy(buff+6, rtkUnionParam, 8);
			break;
		case 1:
			funCode = 0xf1;
			buff[0] = frameHeader;
			buff[1] = funCode;
			buff[buffSize-1] = frameTail;
			memcpy(buff+2, SpeedUnionParam, 4);
			break;
	}
	
	memcpy(sendDataBuf, buff, buffSize);
	memset(buff, 0, buffSize);
}

// 将接收到的上位机数据解包
void bigCarMotorDataUnpack(uint8_t type, hallSensorUnion *hallSensorUnionParam, gpsUnion *gpsUnionParam, uint8_t *DataBuf)
{
	switch(type)
	{
		case 0:   //F2
			memcpy(&hallSensorUnionParam[0], DataBuf+2, 2);
			memcpy(&hallSensorUnionParam[1], DataBuf+4, 2);
			
			memcpy(&gpsUnionParam[0], DataBuf+6, 4);
			memcpy(&gpsUnionParam[1], DataBuf+10, 4);
			break;
		case 1:    //F1
			memcpy(&hallSensorUnionParam[0], DataBuf+2, 2);
			memcpy(&hallSensorUnionParam[1], DataBuf+4, 2);
			break;
	}
}

// 通过串口发送数据到上位机
void sendDataToBigCar(void)
{
	bigCarSpeed sendBigCarSpeedStruct;
	uint8_t sendDataBuf[20] = {0};
	uint8_t dataType = 0;

	getBigCarMotorSpeed();
	sendBigCarSpeedStruct = bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight);
	
	memcpy(SpeedUnionArray, &sendBigCarSpeedStruct.leftMotorSpeed, 2);
	memcpy(SpeedUnionArray+1, &sendBigCarSpeedStruct.rightMotorSpeed, 2);
	
	if(rtkUpdateFlag)   //rtk更新
	{
		rtkUpdateFlag = 0;
		dataType = 0;
		bigCarMotorDataPack(0, SpeedUnionArray, rtkUnionArray, sendDataBuf);
	}
	else   //rtk未更新
	{
		dataType = 1;
		bigCarMotorDataPack(1, SpeedUnionArray, NULL, sendDataBuf);
	}
			
	UART4_Send_BufferData((uint8_t *)sendDataBuf, dataType?7:15);
	
	memset(sendDataBuf, 0, sizeof(sendDataBuf));
}

// 串口中断服务函数
void UART4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	uint8_t Res = 0;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  // 接收中断
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		
    Res = USART_ReceiveData(UART4);  // 读取接收到的数据

		if(Res == 0x55)
		{
			memset(bigCar_ReceiveDataBuf, 0, BIGCAR_RECEIVE_DATA_LEN);
			bufCount = 0;
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
		}
		else if(bigCar_ReceiveDataBuf[0] == 0x55 && (Res == 0xF1 || Res == 0xF2))
		{
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
		}
		else if(bigCar_ReceiveDataBuf[0] == 0x55 && bigCar_ReceiveDataBuf[1] == 0xF1)  // 帧头+功能码   0xF1：数据中没有经纬度数据
		{
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
			if(bigCar_ReceiveDataBuf[6] == 0xAA)    // 帧尾
			{
				// 表示一帧数据接收完成
				bufCount = 0;
				// 设置串口接收事件标志
        xEventGroupSetBitsFromISR(xEventGroup, UART_RX_F1_EVENT_BIT, &xHigherPriorityTaskWoken);
        // 如果需要上下文切换，则进行切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				return;
			}
		}
		else if(bigCar_ReceiveDataBuf[0] == 0x55 && bigCar_ReceiveDataBuf[1] == 0xF2)  // 帧头+功能码   0xF2：数据中有经纬度数据
		{
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
			if(bigCar_ReceiveDataBuf[14] == 0xAA)    // 帧尾
			{
				// 表示一帧数据接收完成
				bufCount = 0;
				// 设置串口接收事件标志
        xEventGroupSetBitsFromISR(xEventGroup, UART_RX_F2_EVENT_BIT, &xHigherPriorityTaskWoken);
        // 如果需要上下文切换，则进行切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				return;
			}
		}
				
		if(bufCount >= BIGCAR_RECEIVE_DATA_LEN)
			bufCount = 0;
	}
}
