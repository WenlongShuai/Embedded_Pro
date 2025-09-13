#include "encoder.h"

/*TIM2初始化为编码器接口*/
void Encoder_Init_TIM(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个引脚初始化的结构体  
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定义一个定时器初始化的结构体
  TIM_ICInitTypeDef TIM_ICInitStructure; //定义一个定时器编码器模式初始化的结构体
	
	RCC_APB1PeriphClockCmd(ENCODER_TIM_RCC, ENABLE); //使能TIM5时钟
	RCC_AHB1PeriphClockCmd(ENCODER_TIM_GPIO_A_RCC | ENCODER_TIM_GPIO_B_RCC, ENABLE); //使能CPIOA时钟
 
	GPIO_PinAFConfig(ENCODER_PORT_A, GPIO_PinSource5, GPIO_AF_TIM2);
	GPIO_PinAFConfig(ENCODER_PORT_B, GPIO_PinSource3, GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_A;	//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ENCODER_PORT_A, &GPIO_InitStructure);	//根据GPIO_InitStructure的参数初始化GPIOA0
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_B;	//PA1
	GPIO_Init(ENCODER_PORT_B, &GPIO_InitStructure);	//根据GPIO_InitStructure的参数初始化GPIOA0

	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; // 预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct的参数初始化定时器TIM5
	
	TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3：CH1、CH2同时计数，四分频
	TIM_ICStructInit(&TIM_ICInitStructure); //把TIM_ICInitStruct 中的每一个参数按缺省值填入
	TIM_ICInitStructure.TIM_ICFilter = 10;  //设置滤波器长度
	TIM_ICInit(ENCODER_TIMx, &TIM_ICInitStructure); //根TIM_ICInitStructure参数初始化定时器TIM5编码器模式

	TIM_Cmd(ENCODER_TIMx, ENABLE); //使能定时器5
}

//读取编码器计数
int Read_Encoder_TIM(void)
{
	int Encoder_TIM;
	Encoder_TIM = ENCODER_TIMx->CNT; //读取计数
	//printf("Encoder_TIM = %d\r\n", Encoder_TIM); 
	if(Encoder_TIM > 0x10D0)Encoder_TIM = Encoder_TIM - 0x20D0; //转化计数值为有方向的值，大于0正转，小于0反转。
	                                                     //TIM4->CNT范围为0-0xffff，初值为0。
	ENCODER_TIMx->CNT = 0; //读取完后计数清零
	return Encoder_TIM; //返回值
}


/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Velocity(void)
{
	int Encoder_TIM;    

	Encoder_TIM = (short)ENCODER_TIMx->CNT;
	ENCODER_TIMx->CNT=0;
	
	return Encoder_TIM;
}
/**************************************************************************
函数功能：读取位置信息
入口参数：定时器
返回  值：位置值
**************************************************************************/
int Read_Position(void)
{
	int Encoder_TIM;    
	Encoder_TIM = (short)ENCODER_TIMx->CNT;
	return Encoder_TIM;
}
