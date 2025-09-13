#include "motor.h"
#include "fuzzy_decision.h"

#define ENCODER_PERIOD (13*4)                                      // 未经过减速器的电机转动一圈产生的脉冲数
#define REDUCTION_RATIO 60                                      // 电机的减速比
#define PULSE_PER_CIRCLE 3120                                  // 理论上PULSE_PER_CIRCLE==ENCODER_PERIOD*REDUCTION_RATIO，但是因为没有电机的参考资料，所以没用ENCODER_PERIOD*REDUCTION_RATIO，PULSE_PER_CIRCLE是实际测量的
#define SPEED_TO_CIRCLE(v) (v / (0.7f * PI) * PULSE_PER_CIRCLE) // 将速度转换为PID采样周期100ms要达到的脉冲数，速度单位为dm/s，车轮的直径为7cm,这里是1s的脉冲数


static void Motor_TIMx_Init(u16 arr, u16 psc);
static void Encoder_Init_TIM3(u16 arr,u16 psc);
static void Encoder_Init_TIM4(u16 arr,u16 psc);


int32_t overflow_cnt0, overflow_cnt1;
int32_t last_cnt0, last_cnt1; // 分别记录定时器1和定时器8的上一次Counter值

extern float RC_Velocity;
extern float Move_X, Move_Y, Move_Z;
extern uint8_t Flag_Left, Flag_Right, Turn_Flag;
extern uint8_t Flag_Direction;

extern float speed_core[SPEED_SUM];

float motorSpeedLeft = 0.0f;
float motorSpeedRight = 0.0f;

/**
@brief      配置电机控制所需的时钟，GPIO口，定时器，中断
@param      None
@retval     None
*/
void Motor_Config(void)
{
	Motor_TIMx_Init(16800-1, 0);   //168MHz / 16800 / 1 = 100us
  Encoder_Init_TIM3(0xFFFF-1, 1-1);  //电机A(左)编码器定时器
	Encoder_Init_TIM4(0xFFFF-1, 1-1);  //电机B(右)编码器定时器
}

/**
@brief      设定左右电机的速度
@param      speed0 设定右电机的速度
            speed1 设定左电机的速度
@retval     None
@note       速度单位为dm/s，速度的正负可以表示方向
*/
void Motor_Set_Speed(float speed0, float speed1)
{
	if (speed0 < 0)
	{
			Motor_Set_Direction(MOTOR_DIRECTION_BACKWARD, MOTOR_LEFT);
			speed0 = -speed0;
	}
	else
	{
			Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_LEFT);
	}
	
	
	if (speed1 < 0)
	{
			Motor_Set_Direction(MOTOR_DIRECTION_BACKWARD, MOTOR_RIGHT);
			speed1 = -speed1;
	}
	else
	{
			Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_RIGHT);
	}

	PID_Set_Point(SPEED_TO_CIRCLE(speed0), SPEED_TO_CIRCLE(speed1)); //PID控制接受的参数是采样周期100ms内所要达到的脉冲数，因此需要对速度进行转换
}


/**
@brief      设定电机的转动方向
@param      dir 可选MOTOR_DIRECTION_FORWARD,MOTOR_DIRECTION_STOP,MOTOR_DIRECTION_BACKWARD,MOTOR_DIRECTION_KEEP
            motor 可选MOTOR_0，MOTOR_1或者两者的组合MOTOR_0|MOTOR_1
@retval     None
*/
void Motor_Set_Direction(MOTOR_DIRECTION dir, uint16_t motor)
{
	switch(dir)
	{
		case MOTOR_DIRECTION_STOP:
			if(motor & 0x01)
			{
				GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 低电平
			}
			else
			{
				GPIO_ResetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // 低电平
			}
			break;
		case MOTOR_DIRECTION_BACKWARD:  //反转，前进
			if(motor & 0x01)
			{
				GPIO_SetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 低电平
			}
			else
			{
				GPIO_SetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // 低电平
			}
			break;
		case MOTOR_DIRECTION_FORWARD:  //正转，后退
			if(motor & 0x01)
			{
				GPIO_SetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 低电平
			}
			else
			{
				GPIO_SetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // 高电平
				GPIO_ResetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // 低电平
			}
			break;
		default:
       break;
	}
}
/**
@brief      获得单个电机的转动方向
@param      motor 可选MOTOR_0，MOTOR_1
@retval     返回电机motor的转动方向
*/
MOTOR_DIRECTION Motor_Get_Direction(uint16_t motor)
{
	GPIO_TypeDef *gpio_Temp[2];
	uint16_t pin_Temp[2];
	
	if(motor & 0x01)   //左电机
	{
		gpio_Temp[0] = MOTOR_AIN1_GPIO;
		pin_Temp[0] = MOTOR_AIN1_PIN;
		
		gpio_Temp[1] = MOTOR_AIN2_GPIO;
		pin_Temp[1] = MOTOR_AIN2_PIN;
	}
	else   //右电机
	{
		gpio_Temp[0] = MOTOR_BIN1_GPIO;
		pin_Temp[0] = MOTOR_BIN1_PIN;
		
		gpio_Temp[1] = MOTOR_BIN2_GPIO;
		pin_Temp[1] = MOTOR_BIN2_PIN;
		
	}
	
	
	if (GPIO_ReadOutputDataBit(gpio_Temp[0], pin_Temp[0]) == RESET && GPIO_ReadOutputDataBit(gpio_Temp[1], pin_Temp[1]) == SET)
			return MOTOR_DIRECTION_FORWARD;
	else if (GPIO_ReadOutputDataBit(gpio_Temp[0], pin_Temp[0]) == SET && GPIO_ReadOutputDataBit(gpio_Temp[1], pin_Temp[1]) == RESET)
			return MOTOR_DIRECTION_BACKWARD;
	else
			return MOTOR_DIRECTION_STOP;
}


/*TIM3初始化为左电机编码器接口*/
static void Encoder_Init_TIM3(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 使能TIM时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能CPIO时钟
 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; // 设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 根据TIM_TimeBaseInitStruct的参数初始化定时器TIM9
	
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);// 使用编码器模式3：CH1、CH2同时计数，四分频
	TIM_ICStructInit(&TIM_ICInitStructure); // 把TIM_ICInitStruct 中的每一个参数按缺省值填入
	TIM_ICInitStructure.TIM_ICFilter = 10;  // 设置滤波器长度
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; // 抢占优先级6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        // 响应优先级0
	NVIC_Init(&NVIC_InitStruct);
			
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			
	TIM_Cmd(TIM3, ENABLE); //使能定时器
}

/* TIM4初始化为右电机编码器接口 */
static void Encoder_Init_TIM4(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 使能TIM时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // 使能CPIO时钟
 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; // 设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);// 使用编码器模式3：CH1、CH2同时计数，四分频
	TIM_ICStructInit(&TIM_ICInitStructure); // 把TIM_ICInitStruct 中的每一个参数按缺省值填入
	TIM_ICInitStructure.TIM_ICFilter = 10;  // 设置滤波器长度
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; // 抢占优先级6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        // 响应优先级0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);


	TIM_Cmd(TIM4, ENABLE); // 使能定时器
}

static void Motor_TIMx_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
  TIM_OCInitTypeDef  TIM_OCInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

  RCC_APB2PeriphClockCmd(MOTOR_PWM_TIMx_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(MOTOR_PWMA_RCC|MOTOR_AIN1_RCC|MOTOR_AIN2_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(MOTOR_PWMB_RCC|MOTOR_BIN1_RCC|MOTOR_BIN2_RCC, ENABLE);

	GPIO_PinAFConfig(MOTOR_PWMA_GPIO, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(MOTOR_PWMB_GPIO, GPIO_PinSource11, GPIO_AF_TIM1);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_PWMA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MOTOR_PWMA_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_PWMB_PIN;
	GPIO_Init(MOTOR_PWMB_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_AIN1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_AIN1_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_AIN2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_AIN2_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_BIN1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_BIN1_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MOTOR_BIN2_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MOTOR_BIN2_GPIO, &GPIO_InitStruct);

	// 配置定时器
	TIM_TimeBaseStruct.TIM_Period = arr;
	TIM_TimeBaseStruct.TIM_Prescaler = psc;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MOTOR_PWM_TIMx, &TIM_TimeBaseStruct);
	
	// 配置定时器得PWM模式---PWMA
  TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(MOTOR_PWM_TIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(MOTOR_PWM_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(MOTOR_PWM_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_PWM_TIMx, ENABLE);
	
	// 配置定时器得PWM模式---PWMB
  TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(MOTOR_PWM_TIMx, &TIM_OCInitStruct);
	TIM_OC2PreloadConfig(MOTOR_PWM_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(MOTOR_PWM_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_PWM_TIMx, ENABLE);
	
  TIM_Cmd(MOTOR_PWM_TIMx, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET)
	{
			overflow_cnt0++;
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) == SET)
	{
			overflow_cnt1++;
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

void getMotorSpeed(void)
{
	switch(Flag_Direction) // 处理方向控制命令
	{ 
		case 1:      Move_X = RC_Velocity/100.0f;  	 Move_Z = 0;         break;   //RC_Velocity/100;  RC_Velocity：串口发送过来的单位是mm，/100是将单位转换成dm
		case 2:      Move_X = 0;      				 		 Move_Z = -PI/2;   	 break;	 
		case 3:      Move_X = -RC_Velocity/100.0f;  	 Move_Z = 0;         break;	 
		case 4:      Move_X = 0;     	 			 	 		 Move_Z = +PI/2;     break;
		default:     Move_X = 0;               		 Move_Z = 0;         break;
	}

	if(Flag_Left == 1)  		Move_Z = PI/2;  // 左自转 
	else if(Flag_Right==1)  Move_Z = -PI/2; // 右自转	

	if(Move_X<0) Move_Z = -Move_Z; // 差速控制原理系列需要此处理
	Move_Z = Move_Z*(RC_Velocity/100.0f);

	// 履带车运动学逆解
	motorSpeedLeft = Move_X - Move_Z * (WHEEL_BASE) / 2.0f;    //计算出左轮的目标速度
	motorSpeedRight = Move_X + Move_Z * (WHEEL_BASE) / 2.0f;    //计算出右轮的目标速度
	
	// 车轮(电机)目标速度限幅
	motorSpeedLeft = target_limit_float( motorSpeedLeft, -speed_core[0], speed_core[0]); 
	motorSpeedRight = target_limit_float( motorSpeedRight, -speed_core[0], speed_core[0]); 
}
