#include "hc_sr04.h"
#include "freeRTOS_TaskList.h"

// ���������ݽṹ������
ultrasonic_sensor_t ultrasonicSensors[SENSORNUM];

//��PG9 �У�PG10 �ң�PG11 ��ࣺPG12 �ҲࣺPG13
static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);        // ʹ������ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                // ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;             // ����̬
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOG,GPIO_Pin_9);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                  
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_10);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                   
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_11);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                    
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;                
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_13);
}


//��ʱ��2���벶������
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//��TIM2_CH1��PA0�� �У�TIM2_CH2��PA1�� �ң�TIM2_CH3��PA2�� ��ࣺTIM2_CH4��PA3��
static void TIM2_CHx_Cap_Init(u32 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //ʹ��GPIOʱ��
	
	// ����PA0ΪTIM2_CH1���ù���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
	// ����PA1ΪTIM2_CH2���ù���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	// ����PA2ΪTIM2_CH3���ù���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);
	// ����PA2ΪTIM2_CH4���ù���
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  // ����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // ����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	// ��ʼ��PA0��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// ��ʼ��PA1��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// ��ʼ��PA2��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// ��ʼ��PA3��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// ��ʼ����ʱ��2 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	// ��ʼ��TIM2���벶��ͨ��1
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // �����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       // ����Ƶ
	TIM_ICInitStructure.TIM_ICFilter = 0x03;                    // �����˲� (0x03 = 3��ʱ������)
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_Update, ENABLE);// ��������ж� ,����CC1IE�����ж�

	
	// ��ʼ��TIM2���벶��ͨ��2
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_Update, ENABLE);// ��������ж� ,����CC2IE�����ж�

	
	// ��ʼ��TIM2���벶��ͨ��3
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC3 | TIM_IT_Update, ENABLE);// ��������ж� ,����CC3IE�����ж�

	
	// ��ʼ��TIM2���벶��ͨ��4
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC4 | TIM_IT_Update, ENABLE);// ��������ж� ,����CC4IE�����ж�

	
	//TIM2�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;  // ��ռ���ȼ�8��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // �����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	 
	TIM_Cmd(TIM2, ENABLE); 	//ʹ�ܶ�ʱ��2
}

//��ʱ��1ͨ��3���벶������
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//�ҲࣺTIM1_CH3��PE13��
static void TIM1_CHx_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// ʹ��TIM1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// ʹ��GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	// ����PA0ΪTIM1_CH3���ù���
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  // ����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // ����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	// ��ʼ��PE13��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// ��ʼ��TIM1ʱ��
	TIM_TimeBaseStructure.TIM_Period = arr;          // �Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;       // Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	// ��ʼ��TIM1���벶��ͨ��3
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // �����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       // ����Ƶ
	TIM_ICInitStructure.TIM_ICFilter = 0x03;                    // �����˲� (0x03 = 3��ʱ������)
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_ITConfig(TIM1, TIM_IT_CC3 | TIM_IT_Update, ENABLE);//��������ж� ,����CC3IE�����ж�

	
	// ����TIM1�жϣ��������벶����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// ����TIM1�жϣ������������
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1, ENABLE);
}

// ��������ʼ��
void Hcsr04Init(void)
{
	TIM2_CHx_Cap_Init(0xFFFF, 84-1);  // TIM2���ã�1MHz����Ƶ�� (84MHz/84 = 1MHz)
	TIM1_CHx_Cap_Init(0xFFFF, 168-1); // TIM1���ã�1MHz����Ƶ�� (168MHz/168 = 1MHz)
	Hcsr04_TRIG_GPIO_Init();
}

// ��������������
void Hcsr04Start(uint8_t num)
{
	if(num == 0)
	{
		GPIO_SetBits(GPIOG, GPIO_Pin_9);
		delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		GPIO_ResetBits(GPIOG, GPIO_Pin_9);
	}
	else if(num == 1)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_10);
		delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		GPIO_ResetBits(GPIOG,GPIO_Pin_10);
	}
	else if(num == 2)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_11);
		delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		GPIO_ResetBits(GPIOG,GPIO_Pin_11);
	}
	else if(num == 3)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_12);
		delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		GPIO_ResetBits(GPIOG,GPIO_Pin_12);
	}
	else 
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_13);
		delay_us(10);//����˵����,��Ҫ�ṩ����10us�ĸߵ�ƽ
		GPIO_ResetBits(GPIOG,GPIO_Pin_13);
	}
}

// ��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{ 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//����1���������¼�
	{
		if(ultrasonicSensors[0].flag == 0)  //������
		{
			ultrasonicSensors[0].start_time = TIM_GetCapture1(TIM2);
			ultrasonicSensors[0].flag = 1;
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC1P=1 ����Ϊ�½��ز���
		}
		else   //�½���
		{
			ultrasonicSensors[0].end_time = TIM_GetCapture1(TIM2);
			// ����ʱ���������end < start��˵�������
      ultrasonicSensors[0].time_diff = (ultrasonicSensors[0].end_time >= ultrasonicSensors[0].start_time) 
        ? (ultrasonicSensors[0].end_time - ultrasonicSensors[0].start_time) 
        : (ultrasonicSensors[0].end_time + 65535 - ultrasonicSensors[0].start_time);
			
			// ֹͣ��ʱ��ʱ��
			xTimerStopFromISR(ultrasonicSensors[0].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[0].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonicSensors[0].time_diff ---> %d\r\n",ultrasonicSensors[0].time_diff);

			// ������Ч����
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[0].sersor_ID
			};

			// �����¼�������
			xQueueSendFromISR(ultrasonicSensors[0].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// �������ݾ����¼�
			xEventGroupSetBitsFromISR(ultrasonicSensors[0].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);

			// �ָ�״̬
      ultrasonicSensors[0].flag = 0;
      ultrasonicSensors[0].overflow_cnt = 0;
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC1P=0 ����Ϊ�����ز���
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); // ����жϱ�־λ
	}
	
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)// ����2���������¼�
	{
		if(ultrasonicSensors[1].flag == 0)  // ������
		{
			ultrasonicSensors[1].start_time = TIM_GetCapture2(TIM2);
			ultrasonicSensors[1].flag = 1;
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC2P=1 ����Ϊ�½��ز���
		}
		else   //�½���
		{
			ultrasonicSensors[1].end_time = TIM_GetCapture2(TIM2);
			// ����ʱ���������end < start��˵�������
      ultrasonicSensors[1].time_diff = (ultrasonicSensors[1].end_time >= ultrasonicSensors[1].start_time) 
        ? (ultrasonicSensors[1].end_time - ultrasonicSensors[1].start_time) 
        : (ultrasonicSensors[1].end_time + 65535 - ultrasonicSensors[1].start_time);
		
			// ֹͣ��ʱ��ʱ�������ж���ֹͣ�����ʱ����Ҫʹ�����⺯����
			xTimerStopFromISR(ultrasonicSensors[1].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[1].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[1].distance ---> %.2f\r\n",ultrasonic_Send[1].distance);
//							printf("ultrasonicSensors[1].time_diff ---> %d\r\n",ultrasonicSensors[1].time_diff);

			// ������Ч����
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[1].sersor_ID
			};
			
			// �����¼�������
			xQueueSendFromISR(ultrasonicSensors[1].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// �������ݾ����¼�
			xEventGroupSetBitsFromISR(ultrasonicSensors[1].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
			
			// �ָ�״̬
      ultrasonicSensors[1].flag = 0;
      ultrasonicSensors[1].overflow_cnt = 0;
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC1P=0 ����Ϊ�����ز���
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2); // ����жϱ�־λ
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)//����3���������¼�
	{
		if(ultrasonicSensors[2].flag == 0)  //������
		{
			ultrasonicSensors[2].start_time = TIM_GetCapture3(TIM2);
			ultrasonicSensors[2].flag = 1;
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC3P=1 ����Ϊ�½��ز���
		}
		else   // �½���
		{
			ultrasonicSensors[2].end_time = TIM_GetCapture3(TIM2);
			// ����ʱ���������end < start��˵�������
      ultrasonicSensors[2].time_diff = (ultrasonicSensors[2].end_time >= ultrasonicSensors[2].start_time) 
        ? (ultrasonicSensors[2].end_time - ultrasonicSensors[2].start_time) 
        : (ultrasonicSensors[2].end_time + 65535 - ultrasonicSensors[2].start_time);
	
			// ֹͣ��ʱ��ʱ�������ж���ֹͣ�����ʱ����Ҫʹ�����⺯����
			xTimerStopFromISR(ultrasonicSensors[2].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[2].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[2].distance ---> %.2f\r\n",ultrasonic_Send[2].distance);
//							printf("ultrasonicSensors[2].time_diff ---> %d\r\n",ultrasonicSensors[2].time_diff);

			// ������Ч����
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[2].sersor_ID
			};
			
			// �����¼�������
			xQueueSendFromISR(ultrasonicSensors[2].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// �������ݾ����¼�
			xEventGroupSetBitsFromISR(ultrasonicSensors[2].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
		
			// �ָ�״̬
      ultrasonicSensors[2].flag = 0;
      ultrasonicSensors[2].overflow_cnt = 0;
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC3P=0 ����Ϊ�����ز���
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3); //����жϱ�־λ
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)// ����4���������¼�
	{
		if(ultrasonicSensors[3].flag == 0)  // ������
		{
			ultrasonicSensors[3].start_time = TIM_GetCapture4(TIM2);
			ultrasonicSensors[3].flag = 1;
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Falling);		//CC4P=1 ����Ϊ�½��ز���
		}
		else   // �½���
		{
			ultrasonicSensors[3].end_time = TIM_GetCapture4(TIM2);
			// ����ʱ���������end < start��˵�������
      ultrasonicSensors[3].time_diff = (ultrasonicSensors[3].end_time >= ultrasonicSensors[3].start_time) 
        ? (ultrasonicSensors[3].end_time - ultrasonicSensors[3].start_time) 
        : (ultrasonicSensors[3].end_time + 65535 - ultrasonicSensors[3].start_time);
	
			// ֹͣ��ʱ��ʱ�������ж���ֹͣ�����ʱ����Ҫʹ�����⺯����
			xTimerStopFromISR(ultrasonicSensors[3].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[3].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[2].distance ---> %.2f\r\n",ultrasonic_Send[2].distance);
//							printf("ultrasonicSensors[2].time_diff ---> %d\r\n",ultrasonicSensors[2].time_diff);


			// ������Ч����
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[3].sersor_ID
			};
			
			// �����¼�������
			xQueueSendFromISR(ultrasonicSensors[3].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// �������ݾ����¼�
			xEventGroupSetBitsFromISR(ultrasonicSensors[3].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
		
			// �ָ�״̬
      ultrasonicSensors[3].flag = 0;
      ultrasonicSensors[3].overflow_cnt = 0;
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Rising); //CC4P=0 ����Ϊ�����ز���
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4); //����жϱ�־λ
	}
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  // �����ж�
	{
		for(int i = 0;i < 4;i++)
		{
			if(ultrasonicSensors[i].flag == 1)
			{
				ultrasonicSensors[i].overflow_cnt++;
			}
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ����жϱ�־λ
	}  
				
	// ����и����ȼ����񱻻��ѣ�ǿ�ƽ����������л�
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}


// ����TIM1�Ĳ���Ƚ��ж�
void TIM1_CC_IRQHandler(void)
{ 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET) // ����3���������¼�
	{
		if(ultrasonicSensors[4].flag == 0)  // ������
		{
			ultrasonicSensors[4].start_time = TIM_GetCapture3(TIM1);
			ultrasonicSensors[4].flag = 1;
			TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Falling);		// CC3P=1 ����Ϊ�½��ز���
		}
		else   // �½���
		{
			ultrasonicSensors[4].end_time = TIM_GetCapture3(TIM1);
			// ����ʱ���������end < start��˵�������
      ultrasonicSensors[4].time_diff = (ultrasonicSensors[4].end_time >= ultrasonicSensors[4].start_time) 
        ? (ultrasonicSensors[4].end_time - ultrasonicSensors[4].start_time) 
        : (ultrasonicSensors[4].end_time + 65535 - ultrasonicSensors[4].start_time);
		
			// ֹͣ��ʱ��ʱ�������ж���ֹͣ�����ʱ����Ҫʹ�����⺯����
			xTimerStopFromISR(ultrasonicSensors[4].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[4].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[3].distance ---> %.2f\r\n",ultrasonic_Send[3].distance);
//							printf("ultrasonicSensors[3].time_diff ---> %d\r\n",ultrasonicSensors[3].time_diff);

			// ������Ч����
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[4].sersor_ID
			};
			
			// �����¼�������
			xQueueSendFromISR(ultrasonicSensors[4].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// �������ݾ����¼�
			xEventGroupSetBitsFromISR(ultrasonicSensors[4].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
			
			// �ָ�״̬
			ultrasonicSensors[4].flag = 0;
			ultrasonicSensors[4].overflow_cnt = 0;
			TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Rising); // CC3P=0 ����Ϊ�����ز���
		}
			
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC3); //����жϱ�־λ
	}
	
	// ����и����ȼ����񱻻��ѣ�ǿ�ƽ����������л�
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

// ����TIM1�ĸ����ж�
void TIM1_UP_TIM10_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// �ж��Ƿ���TIM1�ĸ����ж�
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		if(ultrasonicSensors[4].flag == 1)
		{
			ultrasonicSensors[4].overflow_cnt++;
		}
					
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //����жϱ�־λ
	}
	// ����и����ȼ����񱻻��ѣ�ǿ�ƽ����������л�
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
