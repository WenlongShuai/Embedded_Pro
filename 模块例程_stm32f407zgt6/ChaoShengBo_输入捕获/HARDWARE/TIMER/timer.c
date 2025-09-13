#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////

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


//定时器5通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIMx_CHx_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(TIM_RCC_PERIPH, ENABLE);	//使能TIM2时钟
 	RCC_AHB1PeriphClockCmd(GPIO_RCC_GPIO_PORT, ENABLE);  //使能GPIOA时钟
	GPIO_PinAFConfig(GPIO_GPIO_PORT, GPIO_PinSource_x, GPIO_AF_x);//PA0复用为TIM5

	GPIO_InitStructure.GPIO_Pin =  GPIO_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //引脚设置为下拉，因为PA0对应KEY_UP按键，KEY_UP按键左侧接V3.3
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
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

