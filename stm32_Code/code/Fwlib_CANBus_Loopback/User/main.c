#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_CANBus.h"
#include "bsp_key.h"
#include "string.h"

/* 此实验是对 CAN总线 的环回模式进行测试，因为 CAN总线 的正常模式需要外接一个CAN收发器模块来实现 */

extern CanTxMsg can_Tx_Data;
extern CanRxMsg can_Rx_Data;
extern uint8_t receiveFlag;

int main()
{
	uint8_t mailBox = 0;
	uint8_t data[8] = {'0',101,102,0,104,5,6,7};
	uint8_t i = 0;

	USART_GPIO_Init();
	LED_GPIO_Config();
	Key_GPIO_Config();
	CANx_Init();
	
	LED_B(ON);
	printf("这是一个 CAN总线 通讯实验\r\n");
	printf("按下 KEY1 按键，就可以通过CAN总线发送数据\r\n");
	
	can_Tx_Data.StdId = 0;
	can_Tx_Data.ExtId = CAN_FILTER_PASSID;
	can_Tx_Data.DLC = 8;
	//strcpy函数，遇到到'\0'就停止copy，所以data数组中尽量不要出现0，
	//否则copy到can_Tx_Data.Data的数据就有问题，导致CAN总线发送的数据就有问题。0就是'\0'。
	strcpy((char *)can_Tx_Data.Data,(const char*)data); 
	can_Tx_Data.IDE = CAN_Id_Extended;
	can_Tx_Data.RTR = CAN_RTR_Data;
	
	
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			mailBox = CAN_Transmit(CAN_x, &can_Tx_Data);
			
			while(CAN_TransmitStatus(CAN_x, mailBox) != CAN_TxStatus_Ok);
			
			printf("数据包发送完成\r\n");
		}
		
		if(receiveFlag == 1)
		{
			receiveFlag = 0;
			printf("接收到的数据为:");
			for(i = 0;i < 8;i++)
			{
				printf("%d  ",can_Rx_Data.Data[i]);
			}
			printf("\r\n");
		}
	}
	
	return 0;
}

