#include "motor_PID.h"

#if defined(PID_ASSISTANT_EN)
#include "protocol.h"
#endif

#define T_SAMPLING 0.05f              // 采样时间，单位为秒
#define PD 2.5f                       // 比例带
#define TI 0.4f                       // 积分时间，单位为秒
#define KP (1.0f / PD)                // 比例系数
#define KI (KP * T_SAMPLING / TI)     // 积分系数
#define PID_PWM_MIN 0                 // PWM的最小脉宽
#define PID_PWM_MAX (16800) 					// PWM的最大脉宽
#define MIN_2(i, j) (i < j ? i : j)   // 返回两个数的最小值

extern int32_t overflow_cnt0, overflow_cnt1;
extern int32_t last_cnt0, last_cnt1;
extern float motorSpeedLeft;
extern float motorSpeedRight;

extern uint8_t Flag_Left, Flag_Right, Turn_Flag;
extern uint8_t Flag_Direction;
#if defined(DEBUG_PID)
#define USART_BUFFER_SIZE 100         //每次发送给上位机的最大字节数
int32_t usart_buf[USART_BUFFER_SIZE]; //数据缓冲区
uint32_t usart_buf_cnt;               //缓冲区计数

/**
@brief      将电机脉冲数据通过串口发送到上位机，用于上位机绘制PID图形
@param      None
@retval     None
*/
static void pid_send_to_computer(void)
{
    for (uint32_t i = 0; i < USART_BUFFER_SIZE; ++i)
    {
        usart_send_int32(usart_buf[i]);
        if (i < 50)
            usart_buf[i] = usart_buf[50 + i]; //保留前50个脉冲数据，用于观察连续性
    }
    usart_buf_cnt = 50;
}

/**
@brief      根据函数的形参调整PID的参数和电机的转动方向，用于整定PID控制的参数
@param      point 在采样时间段T_SAMPLING电机光电码盘脉冲数要达到的目标值
@param      pd 比例带，其倒数即为比例系数
@param      ti 积分时间，单位为秒
@param      td 微分时间，单位为秒，本程序使用的是PI控制，因此不需要使用该参数
@param      dir 电机转动方向
@retval     None
@note       在调试PID的参数时才需要使用此函数
*/
void pid_debug_params(uint32_t point, float pd, float ti, float td, MOTOR_DIRECTION dir)
{
    pd = 1 / pd;                            //将比例带pd转换为比例系数
    pid0.proportion = pid1.proportion = pd; //pid0对应的是小车右侧的电机，可以根据需要修改为左侧电机pid1
    pid0.integral = pid1.integral = ti == 0 ? 0 : pd * 0.1f / ti;
    //pid0.derivative=pid1.derivative=pd*td/0.1f;   //本程序使用的是PI控制，因此不需要使用该参数
    pid_set_point(point, point);       //设定脉冲目标值
    motor_set_direction(dir, MOTOR_1); //设置电机转动方向
}

#endif


struct PID_ST pid0, pid1;

static void PID_Tim_Config(void);
static void PID_Struct_Init(struct PID_ST *pid);
static int32_t PID_Increment_Calc(int32_t step, struct PID_ST *pid);

#if defined(PID_ASSISTANT_EN) 
/**
  * @brief  设置比例、积分、微分系数
  * @param  p：比例系数 P
  * @param  i：积分系数 i
  * @param  d：微分系数 d
	*	@note 	无
  * @retval 无
  */
void set_P_I_D(float p, float i, float d)
{   
	pid0.proportion = p; // 设置比例系数 P
	pid0.integral = i;  // 设置积分系数 I
	pid0.last_error = 0;
	pid0.current_point = 0;
	
	pid1.proportion = p;  // 设置比例系数 P
	pid1.integral = i;    // 设置积分系数 I
	pid1.last_error = 0;
	pid1.current_point = 0;
}


/**
  * @brief  设置电机A的目标值
  * @param  val		目标值
	*	@note 	无
  * @retval 无
  */
void set_PID_Target_A(float temp_val)
{ 
	int temp = 0;    // 上位机需要整数参数，转换一下
	set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &temp, 1);     // 给通道 1 发送目标值
	
	Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_LEFT);
	Motor_Set_Direction(MOTOR_DIRECTION_FORWARD, MOTOR_RIGHT);
	
	PID_Set_Point(temp_val/T_SAMPLING, temp_val/T_SAMPLING);  // 设置当前的目标值
}
#endif


/**
@brief      配置PID控制所需的时钟和定时器，并初始化与PID控制有关的结构体
@param      None
@retval     None
@note       在第一次使用PID控制前必须调用此函数进行时钟和定时器的配置，以后不需要调用此函数
*/
void PID_Config(void)
{
	PID_Tim_Config(); //配置PID控制所需的定时器

	//初始化PID控制的结构体
	PID_Struct_Init(&pid0);
	PID_Struct_Init(&pid1);

	#if defined(PID_ASSISTANT_EN)
	float pid_temp[3] = {pid0.proportion, pid0.integral, 0};
	set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // 给通道 1 发送 P I D 值
	#endif
	
}

/**
@brief      通过当前时刻的两个电机的光电码盘脉冲数计算并设置下一时刻两个电机的输出脉宽
@param      step0 右边电机在采样时间段T_SAMPLING的光电码盘脉冲数
@param      step1 左边电机在采样时间段T_SAMPLING的光电码盘脉冲数
@retval     None
*/
void PID_Control(int32_t step0, int32_t step1)
{
	// 因为TIM->CCRx为无符号数，但函数pid_increment_calc可能返回负值，因此必须使用有符号数pwm0和pwm1作为临时变量
	int32_t pwm0 = TIM_GetCapture1(MOTOR_PWM_TIMx);
	int32_t pwm1 = TIM_GetCapture2(MOTOR_PWM_TIMx);	

	//通过增量式PID获得下一时刻的输出脉宽
	pwm0 += PID_Increment_Calc(step0, &pid0);
	pwm1 += PID_Increment_Calc(step1, &pid1);

	// 限制变量pwm的取值在区间[PID_PWM_MIN,PID_PWM_MAX]，因为变量pwm的值最终会赋给TIM5->CCRx
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
	// 通过将变量pwm赋给TIM5->CCRx，控制PWM的输出脉宽，进而影响电机的转速
	TIM_SetCompare1(MOTOR_PWM_TIMx, pwm0);
	TIM_SetCompare2(MOTOR_PWM_TIMx, pwm1);
}

/**
@brief      设置两个电机在一秒要达到的脉冲数
@param      point0 右边电机在一秒要达到的脉冲数
@param      point1 左边电机在一秒要达到的脉冲数
@retval     None
*/
void PID_Set_Point(int32_t point0, int32_t point1)
{
//	printf("point0 --> %d,point1 --> %d\r\n",point0,point1);
    pid0.set_point = MIN_2(point0 * T_SAMPLING, POINT_MAX_LEFT); //point0和point1对应的是一秒要达到的脉冲数，需要将其转换为采样时间T_SAMPLING对应的脉冲数
    pid1.set_point = MIN_2(point1 * T_SAMPLING, POINT_MAX_RIGHT);
	
		#if defined(PID_ASSISTANT_EN) 
		int temp = pid0.set_point;    // 上位机需要整数参数，转换一下
		set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &temp, 1);     // 给通道 1 发送目标值
		#endif
}

/**
@brief      配置PID控制所需的定时器
@param      None
@retval     None
@note       这是一个私有函数
*/
static void PID_Tim_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;               //重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8400-1;            //预分频系数  50ms
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;           //时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);               //使能定时器中断
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;        //使能按键所在的外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //使能外部中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6; //抢占优先级6
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //响应优先级0
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM8, ENABLE);
}


/**
@brief      初始化PID控制的结构体
@param      pid 需要初始化的PID_ST变量
@retval     None
@note       这是一个私有函数；本程序只是用到了PI控制，因此只是初始化与PI控制有关的变量
*/
static void PID_Struct_Init(struct PID_ST *pid)
{
    pid->set_point = pid->last_error = 0;
//    pid->proportion = KP; //根据宏定义KP设置比例系数
//    pid->integral = KI;   //根据宏定义KI设置积分系数
	
	  pid->proportion = 55; //根据宏定义KP设置比例系数
    pid->integral = 3;   //根据宏定义KI设置积分系数
}

/**
@brief      增量式PID算法
@details    此函数实现了增量式PID算法，输入量是电机的光电码盘脉冲数，输出量是控制电机的PWM波形脉宽的增量。通过计算当前时刻输入量与设定目标值的误差，
            获得下一时刻的输出量，如此循环往复，不断减小输入量与设定目标值的误差
@param      step 当前时刻参数pid对应电机的光电码盘的脉冲数
@param      pid 参数step对应的电机的PID控制结构体
@retval     下一时刻控制电机的PWM波形脉宽的增量
@note       这是一个私有函数
*/
static int32_t PID_Increment_Calc(int32_t step, struct PID_ST *pid)
{
    int32_t current_error; //当前时刻光电码盘脉冲数与设定目标值的误差
    int32_t inc;           //下一时刻控制电机的PWM波形脉宽的增量

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

    //增量式PID的计算公式，由于程序只需要PI控制，因此注释掉微分控制部分的计算
    inc = pid->proportion * (current_error - pid->last_error) + pid->integral * current_error; //+pid->derivative*(current_error-2*pid->last_error+pid->prev_error);
    //pid->prev_error=pid->last_error;
    pid->last_error = current_error;

    return inc;
}

/**
@brief      计算脉冲的变化量
@param      current 当前脉冲计数
            last 上一次脉冲计数
            dir 当前电机的转动方向
@retval     脉冲的变化量
@note       根据电机的转动方向有两种计算方式
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
	if(TIM_GetFlagStatus(TIM8, TIM_FLAG_Update) == SET)//定时中断
	{   
		getMotorSpeed();
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight);
		
		int32_t current_cnt0 = TIM_GetCapture1(TIM3); //右边电机的脉冲计数
		int32_t current_cnt1 = TIM_GetCapture1(TIM4); //左边电机的脉冲计数
		
		int32_t tmp0, tmp1;
		
		tmp0 = Get_CNT_Delta(current_cnt0, last_cnt0, Motor_Get_Direction(MOTOR_LEFT)); //右边电机的脉冲变化量
		tmp1 = Get_CNT_Delta(current_cnt1, last_cnt1, Motor_Get_Direction(MOTOR_RIGHT)); //左边电机的脉冲变化量

		last_cnt0 = current_cnt0;
		last_cnt1 = current_cnt1;
		
		#if defined(PID_ASSISTANT_EN) 
		pid0.current_point = 0xffff * overflow_cnt0 + tmp0;
		pid1.current_point = 0xffff * overflow_cnt1 + tmp1;
		#endif
		
		if(Flag_Direction != 0)
		{
			PID_Control(0xffff * overflow_cnt0 + tmp0, 0xffff * overflow_cnt1 + tmp1); //进行PID控制
		}
		overflow_cnt0 = overflow_cnt1 = 0;

		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);                             //===清除定时器8中断标志位
	}
}

