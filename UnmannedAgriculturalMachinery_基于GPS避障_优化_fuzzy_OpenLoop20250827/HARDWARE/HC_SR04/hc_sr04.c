#include "hc_sr04.h"
#include "freeRTOS_TaskList.h"

// 超声波数据结构体数组
ultrasonic_sensor_t ultrasonicSensors[SENSORNUM];

//左：PG9 中：PG10 右：PG11 左侧：PG12 右侧：PG13
static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);        // 使能外设时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                // 输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // 推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;             // 高阻态
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


//定时器2输入捕获配置
//arr：自动重装值
//psc：时钟预分频数
//左：TIM2_CH1（PA0） 中：TIM2_CH2（PA1） 右：TIM2_CH3（PA2） 左侧：TIM2_CH4（PA3）
static void TIM2_CHx_Cap_Init(u32 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //使能GPIO时钟
	
	// 配置PA0为TIM2_CH1复用功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
	// 配置PA1为TIM2_CH2复用功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	// 配置PA2为TIM2_CH3复用功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);
	// 配置PA2为TIM2_CH4复用功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  // 复用模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // 上拉模式，减少噪声干扰
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	// 初始化PA0的GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 初始化PA1的GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 初始化PA2的GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 初始化PA3的GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 初始化定时器2 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	// 初始化TIM2输入捕获通道1
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       // 不分频
	TIM_ICInitStructure.TIM_ICFilter = 0x03;                    // 增加滤波 (0x03 = 3个时钟周期)
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_Update, ENABLE);// 允许更新中断 ,允许CC1IE捕获中断

	
	// 初始化TIM2输入捕获通道2
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_Update, ENABLE);// 允许更新中断 ,允许CC2IE捕获中断

	
	// 初始化TIM2输入捕获通道3
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC3 | TIM_IT_Update, ENABLE);// 允许更新中断 ,允许CC3IE捕获中断

	
	// 初始化TIM2输入捕获通道4
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_CC4 | TIM_IT_Update, ENABLE);// 允许更新中断 ,允许CC4IE捕获中断

	
	//TIM2中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;  // 抢占优先级8级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // 从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	 
	TIM_Cmd(TIM2, ENABLE); 	//使能定时器2
}

//定时器1通道3输入捕获配置
//arr：自动重装值
//psc：时钟预分频数
//右侧：TIM1_CH3（PE13）
static void TIM1_CHx_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 使能TIM1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// 使能GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	// 配置PA0为TIM1_CH3复用功能
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  // 复用模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // 上拉模式，减少噪声干扰
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	// 初始化PE13的GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// 初始化TIM1时基
	TIM_TimeBaseStructure.TIM_Period = arr;          // 自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;       // 预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	// 初始化TIM1输入捕获通道3
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       // 不分频
	TIM_ICInitStructure.TIM_ICFilter = 0x03;                    // 增加滤波 (0x03 = 3个时钟周期)
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_ITConfig(TIM1, TIM_IT_CC3 | TIM_IT_Update, ENABLE);//允许更新中断 ,允许CC3IE捕获中断

	
	// 配置TIM1中断（用于输入捕获处理）
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// 配置TIM1中断（用于溢出处理）
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1, ENABLE);
}

// 超声波初始化
void Hcsr04Init(void)
{
	TIM2_CHx_Cap_Init(0xFFFF, 84-1);  // TIM2配置：1MHz计数频率 (84MHz/84 = 1MHz)
	TIM1_CHx_Cap_Init(0xFFFF, 168-1); // TIM1配置：1MHz计数频率 (168MHz/168 = 1MHz)
	Hcsr04_TRIG_GPIO_Init();
}

// 超声波触发测量
void Hcsr04Start(uint8_t num)
{
	if(num == 0)
	{
		GPIO_SetBits(GPIOG, GPIO_Pin_9);
		delay_us(10);//根据说明书,需要提供至少10us的高电平
		GPIO_ResetBits(GPIOG, GPIO_Pin_9);
	}
	else if(num == 1)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_10);
		delay_us(10);//根据说明书,需要提供至少10us的高电平
		GPIO_ResetBits(GPIOG,GPIO_Pin_10);
	}
	else if(num == 2)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_11);
		delay_us(10);//根据说明书,需要提供至少10us的高电平
		GPIO_ResetBits(GPIOG,GPIO_Pin_11);
	}
	else if(num == 3)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_12);
		delay_us(10);//根据说明书,需要提供至少10us的高电平
		GPIO_ResetBits(GPIOG,GPIO_Pin_12);
	}
	else 
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_13);
		delay_us(10);//根据说明书,需要提供至少10us的高电平
		GPIO_ResetBits(GPIOG,GPIO_Pin_13);
	}
}

// 定时器2中断服务函数
void TIM2_IRQHandler(void)
{ 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
	{
		if(ultrasonicSensors[0].flag == 0)  //上升沿
		{
			ultrasonicSensors[0].start_time = TIM_GetCapture1(TIM2);
			ultrasonicSensors[0].flag = 1;
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC1P=1 设置为下降沿捕获
		}
		else   //下降沿
		{
			ultrasonicSensors[0].end_time = TIM_GetCapture1(TIM2);
			// 处理定时器溢出（若end < start，说明溢出）
      ultrasonicSensors[0].time_diff = (ultrasonicSensors[0].end_time >= ultrasonicSensors[0].start_time) 
        ? (ultrasonicSensors[0].end_time - ultrasonicSensors[0].start_time) 
        : (ultrasonicSensors[0].end_time + 65535 - ultrasonicSensors[0].start_time);
			
			// 停止超时定时器
			xTimerStopFromISR(ultrasonicSensors[0].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[0].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonicSensors[0].time_diff ---> %d\r\n",ultrasonicSensors[0].time_diff);

			// 发送有效数据
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[0].sersor_ID
			};

			// 发送事件到队列
			xQueueSendFromISR(ultrasonicSensors[0].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// 设置数据就绪事件
			xEventGroupSetBitsFromISR(ultrasonicSensors[0].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);

			// 恢复状态
      ultrasonicSensors[0].flag = 0;
      ultrasonicSensors[0].overflow_cnt = 0;
			TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC1P=0 设置为上升沿捕获
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); // 清除中断标志位
	}
	
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)// 捕获2发生捕获事件
	{
		if(ultrasonicSensors[1].flag == 0)  // 上升沿
		{
			ultrasonicSensors[1].start_time = TIM_GetCapture2(TIM2);
			ultrasonicSensors[1].flag = 1;
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC2P=1 设置为下降沿捕获
		}
		else   //下降沿
		{
			ultrasonicSensors[1].end_time = TIM_GetCapture2(TIM2);
			// 处理定时器溢出（若end < start，说明溢出）
      ultrasonicSensors[1].time_diff = (ultrasonicSensors[1].end_time >= ultrasonicSensors[1].start_time) 
        ? (ultrasonicSensors[1].end_time - ultrasonicSensors[1].start_time) 
        : (ultrasonicSensors[1].end_time + 65535 - ultrasonicSensors[1].start_time);
		
			// 停止超时定时器（在中断中停止软件定时器需要使用特殊函数）
			xTimerStopFromISR(ultrasonicSensors[1].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[1].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[1].distance ---> %.2f\r\n",ultrasonic_Send[1].distance);
//							printf("ultrasonicSensors[1].time_diff ---> %d\r\n",ultrasonicSensors[1].time_diff);

			// 发送有效数据
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[1].sersor_ID
			};
			
			// 发送事件到队列
			xQueueSendFromISR(ultrasonicSensors[1].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// 设置数据就绪事件
			xEventGroupSetBitsFromISR(ultrasonicSensors[1].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
			
			// 恢复状态
      ultrasonicSensors[1].flag = 0;
      ultrasonicSensors[1].overflow_cnt = 0;
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC1P=0 设置为上升沿捕获
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2); // 清除中断标志位
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
	{
		if(ultrasonicSensors[2].flag == 0)  //上升沿
		{
			ultrasonicSensors[2].start_time = TIM_GetCapture3(TIM2);
			ultrasonicSensors[2].flag = 1;
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Falling);		// CC3P=1 设置为下降沿捕获
		}
		else   // 下降沿
		{
			ultrasonicSensors[2].end_time = TIM_GetCapture3(TIM2);
			// 处理定时器溢出（若end < start，说明溢出）
      ultrasonicSensors[2].time_diff = (ultrasonicSensors[2].end_time >= ultrasonicSensors[2].start_time) 
        ? (ultrasonicSensors[2].end_time - ultrasonicSensors[2].start_time) 
        : (ultrasonicSensors[2].end_time + 65535 - ultrasonicSensors[2].start_time);
	
			// 停止超时定时器（在中断中停止软件定时器需要使用特殊函数）
			xTimerStopFromISR(ultrasonicSensors[2].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[2].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[2].distance ---> %.2f\r\n",ultrasonic_Send[2].distance);
//							printf("ultrasonicSensors[2].time_diff ---> %d\r\n",ultrasonicSensors[2].time_diff);

			// 发送有效数据
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[2].sersor_ID
			};
			
			// 发送事件到队列
			xQueueSendFromISR(ultrasonicSensors[2].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// 设置数据就绪事件
			xEventGroupSetBitsFromISR(ultrasonicSensors[2].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
		
			// 恢复状态
      ultrasonicSensors[2].flag = 0;
      ultrasonicSensors[2].overflow_cnt = 0;
			TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising); // CC3P=0 设置为上升沿捕获
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3); //清除中断标志位
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)// 捕获4发生捕获事件
	{
		if(ultrasonicSensors[3].flag == 0)  // 上升沿
		{
			ultrasonicSensors[3].start_time = TIM_GetCapture4(TIM2);
			ultrasonicSensors[3].flag = 1;
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Falling);		//CC4P=1 设置为下降沿捕获
		}
		else   // 下降沿
		{
			ultrasonicSensors[3].end_time = TIM_GetCapture4(TIM2);
			// 处理定时器溢出（若end < start，说明溢出）
      ultrasonicSensors[3].time_diff = (ultrasonicSensors[3].end_time >= ultrasonicSensors[3].start_time) 
        ? (ultrasonicSensors[3].end_time - ultrasonicSensors[3].start_time) 
        : (ultrasonicSensors[3].end_time + 65535 - ultrasonicSensors[3].start_time);
	
			// 停止超时定时器（在中断中停止软件定时器需要使用特殊函数）
			xTimerStopFromISR(ultrasonicSensors[3].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[3].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[2].distance ---> %.2f\r\n",ultrasonic_Send[2].distance);
//							printf("ultrasonicSensors[2].time_diff ---> %d\r\n",ultrasonicSensors[2].time_diff);


			// 发送有效数据
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[3].sersor_ID
			};
			
			// 发送事件到队列
			xQueueSendFromISR(ultrasonicSensors[3].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// 设置数据就绪事件
			xEventGroupSetBitsFromISR(ultrasonicSensors[3].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
		
			// 恢复状态
      ultrasonicSensors[3].flag = 0;
      ultrasonicSensors[3].overflow_cnt = 0;
			TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Rising); //CC4P=0 设置为上升沿捕获
		}
			
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4); //清除中断标志位
	}
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  // 更新中断
	{
		for(int i = 0;i < 4;i++)
		{
			if(ultrasonicSensors[i].flag == 1)
			{
				ultrasonicSensors[i].overflow_cnt++;
			}
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位
	}  
				
	// 如果有高优先级任务被唤醒，强制进行上下文切换
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}


// 处理TIM1的捕获比较中断
void TIM1_CC_IRQHandler(void)
{ 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET) // 捕获3发生捕获事件
	{
		if(ultrasonicSensors[4].flag == 0)  // 上升沿
		{
			ultrasonicSensors[4].start_time = TIM_GetCapture3(TIM1);
			ultrasonicSensors[4].flag = 1;
			TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Falling);		// CC3P=1 设置为下降沿捕获
		}
		else   // 下降沿
		{
			ultrasonicSensors[4].end_time = TIM_GetCapture3(TIM1);
			// 处理定时器溢出（若end < start，说明溢出）
      ultrasonicSensors[4].time_diff = (ultrasonicSensors[4].end_time >= ultrasonicSensors[4].start_time) 
        ? (ultrasonicSensors[4].end_time - ultrasonicSensors[4].start_time) 
        : (ultrasonicSensors[4].end_time + 65535 - ultrasonicSensors[4].start_time);
		
			// 停止超时定时器（在中断中停止软件定时器需要使用特殊函数）
			xTimerStopFromISR(ultrasonicSensors[4].timeout_timer, &xHigherPriorityTaskWoken);
			
			float distance = ultrasonicSensors[4].time_diff * 0.01715f;   // 0.0343/2=0.0171
//				printf("ultrasonic[3].distance ---> %.2f\r\n",ultrasonic_Send[3].distance);
//							printf("ultrasonicSensors[3].time_diff ---> %d\r\n",ultrasonicSensors[3].time_diff);

			// 发送有效数据
			ultrasonic_data_t data = {
					.distance_cm = distance,
					.is_valid = 1,
					.sensor_id = ultrasonicSensors[4].sersor_ID
			};
			
			// 发送事件到队列
			xQueueSendFromISR(ultrasonicSensors[4].data_queue, &data, &xHigherPriorityTaskWoken);
			
			// 设置数据就绪事件
			xEventGroupSetBitsFromISR(ultrasonicSensors[4].event_group, ULTRASONIC_DATA_READY_BIT, &xHigherPriorityTaskWoken);
			
			// 恢复状态
			ultrasonicSensors[4].flag = 0;
			ultrasonicSensors[4].overflow_cnt = 0;
			TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Rising); // CC3P=0 设置为上升沿捕获
		}
			
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC3); //清除中断标志位
	}
	
	// 如果有高优先级任务被唤醒，强制进行上下文切换
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

// 处理TIM1的更新中断
void TIM1_UP_TIM10_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// 判断是否是TIM1的更新中断
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		if(ultrasonicSensors[4].flag == 1)
		{
			ultrasonicSensors[4].overflow_cnt++;
		}
					
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //清除中断标志位
	}
	// 如果有高优先级任务被唤醒，强制进行上下文切换
  if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
