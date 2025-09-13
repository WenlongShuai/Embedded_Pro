#include "motor_PID.h"

#if defined(PID_ASSISTANT_EN)
#include "protocol.h"
#endif

#define T_SAMPLING 0.05f              // ����ʱ�䣬��λΪ��
#define PD 2.5f                       // ������
#define TI 0.4f                       // ����ʱ�䣬��λΪ��
#define KP (1.0f / PD)                // ����ϵ��
#define KI (KP * T_SAMPLING / TI)     // ����ϵ��
#define PID_PWM_MIN 0                 // PWM����С����
#define PID_PWM_MAX (16800) 					// PWM���������
#define MIN_2(i, j) (i < j ? i : j)   // ��������������Сֵ

extern int32_t overflow_cnt0, overflow_cnt1;
extern int32_t last_cnt0, last_cnt1;
extern float motorSpeedLeft;
extern float motorSpeedRight;

extern uint8_t Flag_Left, Flag_Right, Turn_Flag;
extern uint8_t Flag_Direction;
#if defined(DEBUG_PID)
#define USART_BUFFER_SIZE 100         //ÿ�η��͸���λ��������ֽ���
int32_t usart_buf[USART_BUFFER_SIZE]; //���ݻ�����
uint32_t usart_buf_cnt;               //����������

/**
@brief      �������������ͨ�����ڷ��͵���λ����������λ������PIDͼ��
@param      None
@retval     None
*/
static void pid_send_to_computer(void)
{
    for (uint32_t i = 0; i < USART_BUFFER_SIZE; ++i)
    {
        usart_send_int32(usart_buf[i]);
        if (i < 50)
            usart_buf[i] = usart_buf[50 + i]; //����ǰ50���������ݣ����ڹ۲�������
    }
    usart_buf_cnt = 50;
}

/**
@brief      ���ݺ������βε���PID�Ĳ����͵����ת��������������PID���ƵĲ���
@param      point �ڲ���ʱ���T_SAMPLING����������������Ҫ�ﵽ��Ŀ��ֵ
@param      pd ���������䵹����Ϊ����ϵ��
@param      ti ����ʱ�䣬��λΪ��
@param      td ΢��ʱ�䣬��λΪ�룬������ʹ�õ���PI���ƣ���˲���Ҫʹ�øò���
@param      dir ���ת������
@retval     None
@note       �ڵ���PID�Ĳ���ʱ����Ҫʹ�ô˺���
*/
void pid_debug_params(uint32_t point, float pd, float ti, float td, MOTOR_DIRECTION dir)
{
    pd = 1 / pd;                            //��������pdת��Ϊ����ϵ��
    pid0.proportion = pid1.proportion = pd; //pid0��Ӧ����С���Ҳ�ĵ�������Ը�����Ҫ�޸�Ϊ�����pid1
    pid0.integral = pid1.integral = ti == 0 ? 0 : pd * 0.1f / ti;
    //pid0.derivative=pid1.derivative=pd*td/0.1f;   //������ʹ�õ���PI���ƣ���˲���Ҫʹ�øò���
    pid_set_point(point, point);       //�趨����Ŀ��ֵ
    motor_set_direction(dir, MOTOR_1); //���õ��ת������
}

#endif


struct PID_ST pid0, pid1;

static void PID_Tim_Config(void);
static void PID_Struct_Init(struct PID_ST *pid);
static int32_t PID_Increment_Calc(int32_t step, struct PID_ST *pid);

#if defined(PID_ASSISTANT_EN) 
/**
  * @brief  ���ñ��������֡�΢��ϵ��
  * @param  p������ϵ�� P
  * @param  i������ϵ�� i
  * @param  d��΢��ϵ�� d
	*	@note 	��
  * @retval ��
  */
void set_P_I_D(float p, float i, float d)
{   
	pid0.proportion = p; // ���ñ���ϵ�� P
	pid0.integral = i;  // ���û���ϵ�� I
	pid0.last_error = 0;
	pid0.current_point = 0;
	
	pid1.proportion = p;  // ���ñ���ϵ�� P
	pid1.integral = i;    // ���û���ϵ�� I
	pid1.last_error = 0;
	pid1.current_point = 0;
}


/**
  * @brief  ���õ��A��Ŀ��ֵ
  * @param  val		Ŀ��ֵ
	*	@note 	��
  * @retval ��
  */
void set_PID_Target_A(float temp_val)
{ 
	int temp = 0;    // ��λ����Ҫ����������ת��һ��
	set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &temp, 1);     // ��ͨ�� 1 ����Ŀ��ֵ
	
	Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_LEFT);
	Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_RIGHT);
	
	PID_Set_Point(temp_val/T_SAMPLING, temp_val/T_SAMPLING);  // ���õ�ǰ��Ŀ��ֵ
}
#endif


/**
@brief      ����PID���������ʱ�ӺͶ�ʱ��������ʼ����PID�����йصĽṹ��
@param      None
@retval     None
@note       �ڵ�һ��ʹ��PID����ǰ������ô˺�������ʱ�ӺͶ�ʱ�������ã��Ժ���Ҫ���ô˺���
*/
void PID_Config(void)
{
	PID_Tim_Config(); //����PID��������Ķ�ʱ��

	//��ʼ��PID���ƵĽṹ��
	PID_Struct_Init(&pid0);
	PID_Struct_Init(&pid1);

	#if defined(PID_ASSISTANT_EN)
	float pid_temp[3] = {pid0.proportion, pid0.integral, 0};
	set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // ��ͨ�� 1 ���� P I D ֵ
	#endif
	
}

/**
@brief      ͨ����ǰʱ�̵���������Ĺ���������������㲢������һʱ������������������
@param      step0 �ұߵ���ڲ���ʱ���T_SAMPLING�Ĺ������������
@param      step1 ��ߵ���ڲ���ʱ���T_SAMPLING�Ĺ������������
@retval     None
*/
void PID_Control(int32_t step0, int32_t step1)
{
	// ��ΪTIM->CCRxΪ�޷�������������pid_increment_calc���ܷ��ظ�ֵ����˱���ʹ���з�����pwm0��pwm1��Ϊ��ʱ����
	int32_t pwm0 = TIM_GetCapture1(MOTOR_PWM_TIMx);
	int32_t pwm1 = TIM_GetCapture2(MOTOR_PWM_TIMx);	

	//ͨ������ʽPID�����һʱ�̵��������
	pwm0 += PID_Increment_Calc(step0, &pid0);
	pwm1 += PID_Increment_Calc(step1, &pid1);

	// ���Ʊ���pwm��ȡֵ������[PID_PWM_MIN,PID_PWM_MAX]����Ϊ����pwm��ֵ���ջḳ��TIM5->CCRx
	if (pwm0 > PID_PWM_MAX)
	{
			pwm0 = PID_PWM_MAX;
	}
	else if (pwm0 < PID_PWM_MIN)
	{
			pwm0 = PID_PWM_MIN;
	}
	
	if (pwm1 > PID_PWM_MAX)
	{
			pwm1 = PID_PWM_MAX;
	}
	else if (pwm1 < PID_PWM_MIN)
	{
			pwm1 = PID_PWM_MIN;
	}

//	printf("pwm0 = %d,pwm1 = %d\r\n",pwm0,pwm1);
	// ͨ��������pwm����TIM5->CCRx������PWM�������������Ӱ������ת��
	TIM_SetCompare1(MOTOR_PWM_TIMx, pwm0);
	TIM_SetCompare2(MOTOR_PWM_TIMx, pwm1);
}

/**
@brief      �������������һ��Ҫ�ﵽ��������
@param      point0 �ұߵ����һ��Ҫ�ﵽ��������
@param      point1 ��ߵ����һ��Ҫ�ﵽ��������
@retval     None
*/
void PID_Set_Point(int32_t point0, int32_t point1)
{
//	printf("point0 --> %d,point1 --> %d\r\n",point0,point1);
    pid0.set_point = MIN_2(point0 * T_SAMPLING, POINT_MAX_LEFT); //point0��point1��Ӧ����һ��Ҫ�ﵽ������������Ҫ����ת��Ϊ����ʱ��T_SAMPLING��Ӧ��������
    pid1.set_point = MIN_2(point1 * T_SAMPLING, POINT_MAX_RIGHT);
	
		#if defined(PID_ASSISTANT_EN) 
		int temp = pid0.set_point;    // ��λ����Ҫ����������ת��һ��
		set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &temp, 1);     // ��ͨ�� 1 ����Ŀ��ֵ
		#endif
}

/**
@brief      ����PID��������Ķ�ʱ��
@param      None
@retval     None
@note       ����һ��˽�к���
*/
static void PID_Tim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;               //��װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8400-1;            //Ԥ��Ƶϵ��  50ms
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;           //ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);               //ʹ�ܶ�ʱ���ж�
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;        //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ⲿ�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; //��ռ���ȼ�6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //��Ӧ���ȼ�0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM8, ENABLE);
}


/**
@brief      ��ʼ��PID���ƵĽṹ��
@param      pid ��Ҫ��ʼ����PID_ST����
@retval     None
@note       ����һ��˽�к�����������ֻ���õ���PI���ƣ����ֻ�ǳ�ʼ����PI�����йصı���
*/
static void PID_Struct_Init(struct PID_ST *pid)
{
    pid->set_point = pid->last_error = 0;
//    pid->proportion = KP; //���ݺ궨��KP���ñ���ϵ��
//    pid->integral = KI;   //���ݺ궨��KI���û���ϵ��
	
	  pid->proportion = 55; //���ݺ궨��KP���ñ���ϵ��
    pid->integral = 3;   //���ݺ궨��KI���û���ϵ��
}

/**
@brief      ����ʽPID�㷨
@details    �˺���ʵ��������ʽPID�㷨���������ǵ���Ĺ��������������������ǿ��Ƶ����PWM���������������ͨ�����㵱ǰʱ�����������趨Ŀ��ֵ����
            �����һʱ�̵�����������ѭ�����������ϼ�С���������趨Ŀ��ֵ�����
@param      step ��ǰʱ�̲���pid��Ӧ����Ĺ�����̵�������
@param      pid ����step��Ӧ�ĵ����PID���ƽṹ��
@retval     ��һʱ�̿��Ƶ����PWM�������������
@note       ����һ��˽�к���
*/
static int32_t PID_Increment_Calc(int32_t step, struct PID_ST *pid)
{
    int32_t current_error; //��ǰʱ�̹���������������趨Ŀ��ֵ�����
    int32_t inc;           //��һʱ�̿��Ƶ����PWM�������������

#if defined(DEBUG_PID)
    if (pid == &pid1)
    {
        usart_buf[usart_buf_cnt++] = step;
        if (usart_buf_cnt == USART_BUFFER_SIZE)
        {
            pid_send_to_computer();
        }
    }
#endif

     current_error = pid->set_point - step;

    //����ʽPID�ļ��㹫ʽ�����ڳ���ֻ��ҪPI���ƣ����ע�͵�΢�ֿ��Ʋ��ֵļ���
    inc = pid->proportion * (current_error - pid->last_error) + pid->integral * current_error; //+pid->derivative*(current_error-2*pid->last_error+pid->prev_error);
    //pid->prev_error=pid->last_error;
    pid->last_error = current_error;

    return inc;
}

/**
@brief      ��������ı仯��
@param      current ��ǰ�������
            last ��һ���������
            dir ��ǰ�����ת������
@retval     ����ı仯��
@note       ���ݵ����ת�����������ּ��㷽ʽ
*/
int32_t Get_CNT_Delta(int32_t current, int32_t last, MOTOR_DIRECTION dir)
{
    switch (dir)
    {
    case MOTOR_DIRECTION_FORWARD:
        return current - last;
    case MOTOR_DIRECTION_BACKWARD:
        return last - current;
    default:
        return current - last;
    }
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM8, TIM_FLAG_Update) == SET)//��ʱ�ж�
	{   
		getMotorSpeed();
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight);
		
		int32_t current_cnt0 = TIM_GetCapture1(TIM3); //�ұߵ�����������
		int32_t current_cnt1 = TIM_GetCapture1(TIM4); //��ߵ�����������
		
		int32_t tmp0, tmp1;
		
		tmp0 = Get_CNT_Delta(current_cnt0, last_cnt0, Motor_Get_Direction(MOTOR_LEFT)); //�ұߵ��������仯��
		tmp1 = Get_CNT_Delta(current_cnt1, last_cnt1, Motor_Get_Direction(MOTOR_RIGHT)); //��ߵ��������仯��

		last_cnt0 = current_cnt0;
		last_cnt1 = current_cnt1;
		
		#if defined(PID_ASSISTANT_EN) 
		pid0.current_point = 0xffff * overflow_cnt0 + tmp0;
		pid1.current_point = 0xffff * overflow_cnt1 + tmp1;
		#endif
		
		if(Flag_Direction != 0)
		{
			PID_Control(0xffff * overflow_cnt0 + tmp0, 0xffff * overflow_cnt1 + tmp1); //����PID����
		}
		overflow_cnt0 = overflow_cnt1 = 0;

		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);                             //===�����ʱ��8�жϱ�־λ
	}
}

