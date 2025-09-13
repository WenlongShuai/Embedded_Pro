#include "bigCarMotorControl.h"
#include "fun.h"
#include <string.h>


extern float RC_Velocity;
extern float Move_X, Move_Y, Move_Z;
extern uint8_t Flag_Direction;

float bigCarMotorSpeedLeft;   // �������ٶ�
float bigCarMotorSpeedRight;  // �������ٶ�
SpeedUnion SpeedUnionArray[2] = {0};
rtkUnion rtkUnionArray[2] = {0};
uint8_t rtkUpdateFlag = 0;


// �¼���־����
EventGroupHandle_t xEventGroup = NULL;

uint8_t bigCar_ReceiveDataBuf[BIGCAR_RECEIVE_DATA_LEN] = {0};  // ���մ󳵴�����������
uint8_t bufCount = 0;      // �������ݼ���

static void bigCarMotorControl_USART_GPIO_Init(uint32_t bound)
{
	// GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // ʹ��GPIOʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); // ʹ��USARTʱ��

	//���ڶ�Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4); // ����ΪUART
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); // ����ΪUART
	
	//UART_TX�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // ����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// UART_RX�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

  // UART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// �շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); // ��ʼ������
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	// ��������ж�
	
	//UART4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;// �����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;// ��ռ���ȼ�10
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���
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
@brief      �趨�����ҵ�����ٶ�
@param      speed0 �趨�ҵ�����ٶ�
            speed1 �趨�������ٶ�
@retval     None
@note       �ٶȵ�λΪdm/s���ٶȵ��������Ա�ʾ����
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
	switch(Flag_Direction) // �������������
	{ 
		case 1:      Move_X = RC_Velocity/100.0f;  	 Move_Z = 0;         break;   //RC_Velocity/100;  RC_Velocity�����ڷ��͹����ĵ�λ��mm��/100�ǽ���λת����dm
		case 2:      Move_X = 0;      				 		 Move_Z = -PI/2;   	 break;	 
		case 3:      Move_X = -RC_Velocity/100.0f;  	 Move_Z = 0;         break;	 
		case 4:      Move_X = 0;     	 			 	 		 Move_Z = +PI/2;     break;
		default:     Move_X = 0;               		 Move_Z = 0;         break;
	}

	if(Move_X<0) Move_Z = -Move_Z; // ���ٿ���ԭ��ϵ����Ҫ�˴���
	Move_Z = Move_Z*(RC_Velocity/100.0f);
	
	// �Ĵ����˶�ѧ���
	bigCarMotorSpeedLeft = Move_X - Move_Z / 2.0f;     // ��������ֵ�Ŀ���ٶ�
	bigCarMotorSpeedRight = Move_X + Move_Z / 2.0f;    // ��������ֵ�Ŀ���ٶ�
	
	// ����(���)Ŀ���ٶ��޷�
	bigCarMotorSpeedLeft = target_limit_float( bigCarMotorSpeedLeft, -5, 5); 
	bigCarMotorSpeedRight = target_limit_float( bigCarMotorSpeedRight, -5, 5);
}

// ���͸���λ�������ݴ��
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

// �����յ�����λ�����ݽ��
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

// ͨ�����ڷ������ݵ���λ��
void sendDataToBigCar(void)
{
	bigCarSpeed sendBigCarSpeedStruct;
	uint8_t sendDataBuf[20] = {0};
	uint8_t dataType = 0;

	getBigCarMotorSpeed();
	sendBigCarSpeedStruct = bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight);
	
	memcpy(SpeedUnionArray, &sendBigCarSpeedStruct.leftMotorSpeed, 2);
	memcpy(SpeedUnionArray+1, &sendBigCarSpeedStruct.rightMotorSpeed, 2);
	
	if(rtkUpdateFlag)   //rtk����
	{
		rtkUpdateFlag = 0;
		dataType = 0;
		bigCarMotorDataPack(0, SpeedUnionArray, rtkUnionArray, sendDataBuf);
	}
	else   //rtkδ����
	{
		dataType = 1;
		bigCarMotorDataPack(1, SpeedUnionArray, NULL, sendDataBuf);
	}
			
	UART4_Send_BufferData((uint8_t *)sendDataBuf, dataType?7:15);
	
	memset(sendDataBuf, 0, sizeof(sendDataBuf));
}

// �����жϷ�����
void UART4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	uint8_t Res = 0;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  // �����ж�
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		
    Res = USART_ReceiveData(UART4);  // ��ȡ���յ�������

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
		else if(bigCar_ReceiveDataBuf[0] == 0x55 && bigCar_ReceiveDataBuf[1] == 0xF1)  // ֡ͷ+������   0xF1��������û�о�γ������
		{
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
			if(bigCar_ReceiveDataBuf[6] == 0xAA)    // ֡β
			{
				// ��ʾһ֡���ݽ������
				bufCount = 0;
				// ���ô��ڽ����¼���־
        xEventGroupSetBitsFromISR(xEventGroup, UART_RX_F1_EVENT_BIT, &xHigherPriorityTaskWoken);
        // �����Ҫ�������л���������л�
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				return;
			}
		}
		else if(bigCar_ReceiveDataBuf[0] == 0x55 && bigCar_ReceiveDataBuf[1] == 0xF2)  // ֡ͷ+������   0xF2���������о�γ������
		{
			bigCar_ReceiveDataBuf[bufCount] = Res;
			bufCount++;
			if(bigCar_ReceiveDataBuf[14] == 0xAA)    // ֡β
			{
				// ��ʾһ֡���ݽ������
				bufCount = 0;
				// ���ô��ڽ����¼���־
        xEventGroupSetBitsFromISR(xEventGroup, UART_RX_F2_EVENT_BIT, &xHigherPriorityTaskWoken);
        // �����Ҫ�������л���������л�
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				return;
			}
		}
				
		if(bufCount >= BIGCAR_RECEIVE_DATA_LEN)
			bufCount = 0;
	}
}
