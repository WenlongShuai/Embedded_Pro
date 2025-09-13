#include "key.h"

uint8_t keyStateFlag = 0;
uint16_t time = 0;  //定时器定时时间，按键释放和按下时间的获取
uint8_t key_Process_Time = 0;  //状态机处理时间,10ms调用一次状态机
Key_t key0;

// 中断优先级配置
static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 	
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            都有
 *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
 *  TIM_Period               都有
 *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 */

static void BASIC_TIM_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		// 开启定时器时钟,即内部时钟CK_INT=168M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
		// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = (1000-1);	

	  // 时钟预分频数为
    TIM_TimeBaseStructure.TIM_Prescaler= 84-1;  //1ms
	
		// 时钟分频因子 ，基本定时器没有，不用管
    //TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		
		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
    //TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		
		// 重复计数器的值，基本定时器没有，不用管
		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // 初始化定时器
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
		
		// 清除计数器中断标志位
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	  
		// 开启计数器中断
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
		
		// 使能计数器
    TIM_Cmd(TIM6, ENABLE);	
}

static void BASIC_TIM_Init(void)
{
	BASIC_TIM_Mode_Config();
	BASIC_TIM_NVIC_Config();
}

/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);                      //使能 PORTE 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //上拉输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3; 			//PE4--K0   PE3--K1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
  // 将GPIO连接到EXTI线路（STM32F4使用SYSCFG配置）
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	// 配置EXTI
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// 配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// 初始化按键结构体
	key0.state = KEY_STATE_IDLE;
	key0.event = KEY_EVENT_NONE;
	key0.pressTime = 0;
	key0.releaseTime = 0;
	key0.pressed = 0;
	
	// 初始化定时器
	BASIC_TIM_Init();
} 

//读取按键电平（低电平表示按下）
static uint8_t Key_Read_K0(void) 
{
	return (GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == 0);
}

//读取按键电平（低电平表示按下）
static uint8_t Key_Read_K1(void) 
{
	return (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == 0);
}

// 获取系统时间（毫秒）
static uint32_t GetSystemTimeMs(void) 
{
	if(time >=0xFFFF)
		time = 0;
	return time; // 示例实现，实际应使用定时器
}


// 按键状态机处理（需在定时中断中调用，例如10ms调用一次）
void Key_Process_K0(void) 
{
    uint32_t currentTime = GetSystemTimeMs();
    uint8_t keyLevel = Key_Read_K0();
    
    // 检测按键状态变化
    if (keyLevel != key0.pressed) {
        // 防抖处理：延时20ms确认
        if (GetSystemTimeMs() - key0.releaseTime > 20) {
            key0.pressed = keyLevel;
            if (key0.pressed) {
                key0.pressTime = currentTime;
            } else {
                key0.releaseTime = currentTime;
            }
        }
    }
    
    // 状态机处理
    switch (key0.state) {
        case KEY_STATE_IDLE:
            if (key0.pressed) {
                key0.state = KEY_STATE_PRESSED;
            }
            key0.event = KEY_EVENT_NONE;
            break;
            
        case KEY_STATE_PRESSED:
            if (!key0.pressed) {
                // 短按释放
                key0.state = KEY_STATE_SHORT_RELEASED;
            } else if (currentTime - key0.pressTime > 2000) {
                // 长按（超过2S）
                key0.state = KEY_STATE_LONG_PRESS;
                key0.event = KEY_EVENT_LONG_PRESS;
            }
            break;
            
        case KEY_STATE_SHORT_RELEASED:
            key0.state = KEY_STATE_WAIT_DOUBLE;
            break;
            
        case KEY_STATE_WAIT_DOUBLE:
            if (key0.pressed) {
                // 检测到第二次按下
                key0.state = KEY_STATE_DOUBLE_PRESSED;
            } else if (currentTime - key0.releaseTime > 500) {
                // 超时未检测到双击
                key0.state = KEY_STATE_IDLE;
                key0.event = KEY_EVENT_CLICK;
            }
            break;
						
				case KEY_STATE_DOUBLE_PRESSED:
					if (!key0.pressed) {
                // 第二次按下释放，确认为双击
                key0.state = KEY_STATE_IDLE;
                key0.event = KEY_EVENT_DOUBLE_CLICK;
            }
            break;
            
        case KEY_STATE_LONG_PRESS:
            if (!key0.pressed) {
                key0.state = KEY_STATE_IDLE;
            }
            break;
    }
}

void TIM6_DAC_IRQHandler()
{
	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update)==SET)//5ms定时中断
	{   
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);                             //===清除定时器1中断标志位	  
		time++;
		key_Process_Time++;
	}
}

// 中断处理函数
void EXTI4_IRQHandler(void) 
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		// 按键消抖（软件延时）
    for (uint32_t i = 0; i < 40000; i++); // 简单延时消抖
		if(keyStateFlag == 0)
			keyStateFlag = 1;
		// 清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
