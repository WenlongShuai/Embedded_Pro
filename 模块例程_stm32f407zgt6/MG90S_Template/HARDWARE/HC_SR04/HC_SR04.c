#include "HC_SR04.h"

uint8_t flag = 0;				//用于记录中断信号是上升沿还是下降沿
uint32_t number = 0;			//记录定时器中断的次数
uint32_t times = 0;			//记录回响信号的持续时间
float distance = 0;    //计算后的距离

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIMxCHx_CAPTURE_STA = 0;	//输入捕获状态		    				
u16	TIMxCHx_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)

static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_APB2PeriphClockCmd(RCC_TRIG_PERIPH,ENABLE);        //使能GPIOA的外设时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//选择推挽输出模式
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRIG_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}

//定时器2通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
static void TIMx_CHx_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(TIM_RCC_PERIPH, ENABLE);	//使能TIM2时钟
 	RCC_APB2PeriphClockCmd(GPIO_RCC_GPIO_PORT, ENABLE);  //使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(GPIO_GPIO_PORT,GPIO_GPIO_PIN);						 //下拉
	
	//初始化定时器2 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM_x, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM5输入捕获参数
	TIM_ICInitStructure.TIM_Channel = TIMx_CHANNEL_x; //	选择输入端 IC1映射到TI2上
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI2上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM_x, &TIM_ICInitStructure);
	
	//中断分组初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM_x,TIM_IT_Update|TIMx_IT_CCx,ENABLE);//允许更新中断 ,允许CC2IE捕获中断	
	
  TIM_Cmd(TIM_x,ENABLE ); 	//使能定时器2
}


/**
 * @description: 超声波模块的输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init()
{
	TIMx_CHx_Cap_Init(0xFFFF,72-1);
	Hcsr04_TRIG_GPIO_Init();
}

/**
 * @description: HC-SR04触发
 * @param {*}
 * @return {*}
 */
void Hcsr04Start()
{
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
	delay_us(10);//根据说明书,需要提供至少10us的高电平
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
}


/**
 * @description: 读取距离 
 * @param {*}
 * @return {*}
 */
float Hcsr04Read()
{
  // 测距结果限幅
  if(distance >= 400)
  {
    distance = 400;
  }
	else if(distance <= 2)
	{
		distance = 2;
	}
		
  return distance;
}


void TIMx_IRQHANDLER(void)
{ 
 	if((TIMxCHx_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM_x, TIM_IT_Update) != RESET)
		{	    
			if(TIMxCHx_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIMxCHx_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIMxCHx_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIMxCHx_CAPTURE_VAL=0XFFFF;
				}else TIMxCHx_CAPTURE_STA++;
			}	 
		}
		if (TIM_GetITStatus(TIM_x, TIMx_IT_CCx) != RESET)//捕获1发生捕获事件
		{	
			if(TIMxCHx_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIMxCHx_CAPTURE_STA|=0X80;		//标记成功捕获
				TIMxCHx_CAPTURE_VAL=TIM_GetCapturex(TIM_x);
		   	TIM_OCxPolarityConfig(TIM_x,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				TIMxCHx_CAPTURE_STA=0;			//清空
				TIMxCHx_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM_x,0);
				TIMxCHx_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
		   	TIM_OCxPolarityConfig(TIM_x,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}	
		} 
 	}
	TIM_ClearITPendingBit(TIM_x, TIMx_IT_CCx | TIM_IT_Update); //清除中断标志位
}
