#include "key.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "semphr.h"  // �ź���������ͺͺ���������


// ȫ���ź����������ж��������ͬ����
SemaphoreHandle_t xCreateTaskSemaphore = NULL;


/**************************************************************************
�������ܣ�������ʼ��
��ڲ�������
����  ֵ���� 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);                      //ʹ�� PORTE ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //��������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN; 			//PE4--K0   PE3--K1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(KEY0_GPIO_PORT,&GPIO_InitStructure);
	
  // ��GPIO���ӵ�EXTI��·��STM32F4ʹ��SYSCFG���ã�
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	// ����EXTI
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// ����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// �жϴ�����
void EXTI4_IRQHandler(void) 
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		// ���������������ʱ��
		while(GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)){};  //��ȡ������ƽ
		delay_ms(10);   //��ʱ����
		while(GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)){};  //��ȡ������ƽ
//		// �ͷ��ź������жϰ�ȫ��API��
		xSemaphoreGiveFromISR(
				xCreateTaskSemaphore,
				&xHigherPriorityTaskWoken
		);

		// ����жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
	
	// �����������л�
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
