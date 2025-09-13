#include "HC_SR04.h"

extern float distance;

static void Hcsr04_TRIG_GPIO_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_TRIG_PERIPH,ENABLE);        //使能GPIOA的外设时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;                             //选择推挽输出模式
	GPIO_InitStructure.GPIO_Pin=TRIG_PIN;                                    //选择GPIO_Pin_2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;                            //默认选择50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //无上拉
	GPIO_Init(TRIG_PORT,&GPIO_InitStructure);
}

/**
 * @description: 超声波模块的输入捕获定时器通道初始化
 * @param {TIM_HandleTypeDef} *htim
 * @param {uint32_t} Channel
 * @return {*}
 */
void Hcsr04Init()
{
	Hcsr04_TRIG_GPIO_Init();
	TIMx_Init(100-1,84-1);
}

/**
 * @description: HC-SR04触发
 * @param {*}
 * @return {*}
 */
void Hcsr04Start()
{
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
  delay_us(15);                   //根据说明书,需要提供至少10us的高电平
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
