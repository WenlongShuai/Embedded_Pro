#include "motor.h"
#include "fuzzy_decision.h"

#define ENCODER_PERIOD (13*4)                                      // δ�����������ĵ��ת��һȦ������������
#define REDUCTION_RATIO 60                                      // ����ļ��ٱ�
#define PULSE_PER_CIRCLE 3120                                  // ������PULSE_PER_CIRCLE==ENCODER_PERIOD*REDUCTION_RATIO��������Ϊû�е���Ĳο����ϣ�����û��ENCODER_PERIOD*REDUCTION_RATIO��PULSE_PER_CIRCLE��ʵ�ʲ�����
#define SPEED_TO_CIRCLE(v) (v / (0.7f * PI) * PULSE_PER_CIRCLE) // ���ٶ�ת��ΪPID��������100msҪ�ﵽ�����������ٶȵ�λΪdm/s�����ֵ�ֱ��Ϊ7cm,������1s��������


static void Motor_TIMx_Init(u16 arr, u16 psc);
static void Encoder_Init_TIM3(u16 arr,u16 psc);
static void Encoder_Init_TIM4(u16 arr,u16 psc);


int32_t overflow_cnt0, overflow_cnt1;
int32_t last_cnt0, last_cnt1; // �ֱ��¼��ʱ��1�Ͷ�ʱ��8����һ��Counterֵ

extern float RC_Velocity;
extern float Move_X, Move_Y, Move_Z;
extern uint8_t Flag_Left, Flag_Right, Turn_Flag;
extern uint8_t Flag_Direction;

extern float speed_core[SPEED_SUM];

float motorSpeedLeft = 0.0f;
float motorSpeedRight = 0.0f;

/**
@brief      ���õ�����������ʱ�ӣ�GPIO�ڣ���ʱ�����ж�
@param      None
@retval     None
*/
void Motor_Config(void)
{
	Motor_TIMx_Init(16800-1, 0);   //168MHz / 16800 / 1 = 100us
  Encoder_Init_TIM3(0xFFFF-1, 1-1);  //���A(��)��������ʱ��
	Encoder_Init_TIM4(0xFFFF-1, 1-1);  //���B(��)��������ʱ��
}

/**
@brief      �趨���ҵ�����ٶ�
@param      speed0 �趨�ҵ�����ٶ�
            speed1 �趨�������ٶ�
@retval     None
@note       �ٶȵ�λΪdm/s���ٶȵ��������Ա�ʾ����
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

	PID_Set_Point(SPEED_TO_CIRCLE(speed0), SPEED_TO_CIRCLE(speed1)); //PID���ƽ��ܵĲ����ǲ�������100ms����Ҫ�ﵽ���������������Ҫ���ٶȽ���ת��
}


/**
@brief      �趨�����ת������
@param      dir ��ѡMOTOR_DIRECTION_FORWARD,MOTOR_DIRECTION_STOP,MOTOR_DIRECTION_BACKWARD,MOTOR_DIRECTION_KEEP
            motor ��ѡMOTOR_0��MOTOR_1�������ߵ����MOTOR_0|MOTOR_1
@retval     None
*/
void Motor_Set_Direction(MOTOR_DIRECTION dir, uint16_t motor)
{
	switch(dir)
	{
		case MOTOR_DIRECTION_STOP:
			if(motor & 0x01)
			{
				GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // �͵�ƽ
			}
			else
			{
				GPIO_ResetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // �͵�ƽ
			}
			break;
		case MOTOR_DIRECTION_BACKWARD:  //��ת��ǰ��
			if(motor & 0x01)
			{
				GPIO_SetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // �͵�ƽ
			}
			else
			{
				GPIO_SetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // �͵�ƽ
			}
			break;
		case MOTOR_DIRECTION_FORWARD:  //��ת������
			if(motor & 0x01)
			{
				GPIO_SetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // �͵�ƽ
			}
			else
			{
				GPIO_SetBits(MOTOR_BIN2_GPIO, MOTOR_BIN2_PIN);	 // �ߵ�ƽ
				GPIO_ResetBits(MOTOR_BIN1_GPIO, MOTOR_BIN1_PIN);	 // �͵�ƽ
			}
			break;
		default:
       break;
	}
}
/**
@brief      ��õ��������ת������
@param      motor ��ѡMOTOR_0��MOTOR_1
@retval     ���ص��motor��ת������
*/
MOTOR_DIRECTION Motor_Get_Direction(uint16_t motor)
{
	GPIO_TypeDef *gpio_Temp[2];
	uint16_t pin_Temp[2];
	
	if(motor & 0x01)   //����
	{
		gpio_Temp[0] = MOTOR_AIN1_GPIO;
		pin_Temp[0] = MOTOR_AIN1_PIN;
		
		gpio_Temp[1] = MOTOR_AIN2_GPIO;
		pin_Temp[1] = MOTOR_AIN2_PIN;
	}
	else   //�ҵ��
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


/*TIM3��ʼ��Ϊ�����������ӿ�*/
static void Encoder_Init_TIM3(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // ʹ��TIMʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // ʹ��CPIOʱ��
 
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

	TIM_TimeBaseStructure.TIM_Period = arr; // �趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // ����TIM_TimeBaseInitStruct�Ĳ�����ʼ����ʱ��TIM9
	
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);// ʹ�ñ�����ģʽ3��CH1��CH2ͬʱ�������ķ�Ƶ
	TIM_ICStructInit(&TIM_ICInitStructure); // ��TIM_ICInitStruct �е�ÿһ��������ȱʡֵ����
	TIM_ICInitStructure.TIM_ICFilter = 10;  // �����˲�������
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; // ��ռ���ȼ�6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        // ��Ӧ���ȼ�0
	NVIC_Init(&NVIC_InitStruct);
			
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			
	TIM_Cmd(TIM3, ENABLE); //ʹ�ܶ�ʱ��
}

/* TIM4��ʼ��Ϊ�ҵ���������ӿ� */
static void Encoder_Init_TIM4(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // ʹ��TIMʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // ʹ��CPIOʱ��
 
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

	TIM_TimeBaseStructure.TIM_Period = arr; // �趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);// ʹ�ñ�����ģʽ3��CH1��CH2ͬʱ�������ķ�Ƶ
	TIM_ICStructInit(&TIM_ICInitStructure); // ��TIM_ICInitStruct �е�ÿһ��������ȱʡֵ����
	TIM_ICInitStructure.TIM_ICFilter = 10;  // �����˲�������
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; // ��ռ���ȼ�6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        // ��Ӧ���ȼ�0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);


	TIM_Cmd(TIM4, ENABLE); // ʹ�ܶ�ʱ��
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

	// ���ö�ʱ��
	TIM_TimeBaseStruct.TIM_Period = arr;
	TIM_TimeBaseStruct.TIM_Prescaler = psc;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MOTOR_PWM_TIMx, &TIM_TimeBaseStruct);
	
	// ���ö�ʱ����PWMģʽ---PWMA
  TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(MOTOR_PWM_TIMx, &TIM_OCInitStruct);
	TIM_OC1PreloadConfig(MOTOR_PWM_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(MOTOR_PWM_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_PWM_TIMx, ENABLE);
	
	// ���ö�ʱ����PWMģʽ---PWMB
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
	switch(Flag_Direction) // �������������
	{ 
		case 1:      Move_X = RC_Velocity/100.0f;  	 Move_Z = 0;         break;   //RC_Velocity/100;  RC_Velocity�����ڷ��͹����ĵ�λ��mm��/100�ǽ���λת����dm
		case 2:      Move_X = 0;      				 		 Move_Z = -PI/2;   	 break;	 
		case 3:      Move_X = -RC_Velocity/100.0f;  	 Move_Z = 0;         break;	 
		case 4:      Move_X = 0;     	 			 	 		 Move_Z = +PI/2;     break;
		default:     Move_X = 0;               		 Move_Z = 0;         break;
	}

	if(Flag_Left == 1)  		Move_Z = PI/2;  // ����ת 
	else if(Flag_Right==1)  Move_Z = -PI/2; // ����ת	

	if(Move_X<0) Move_Z = -Move_Z; // ���ٿ���ԭ��ϵ����Ҫ�˴���
	Move_Z = Move_Z*(RC_Velocity/100.0f);

	// �Ĵ����˶�ѧ���
	motorSpeedLeft = Move_X - Move_Z * (WHEEL_BASE) / 2.0f;    //��������ֵ�Ŀ���ٶ�
	motorSpeedRight = Move_X + Move_Z * (WHEEL_BASE) / 2.0f;    //��������ֵ�Ŀ���ٶ�
	
	// ����(���)Ŀ���ٶ��޷�
	motorSpeedLeft = target_limit_float( motorSpeedLeft, -speed_core[0], speed_core[0]); 
	motorSpeedRight = target_limit_float( motorSpeedRight, -speed_core[0], speed_core[0]); 
}
