#include "HC_SR04.h"

extern float distance;

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
	Delay_us(10);//根据说明书,需要提供至少10us的高电平
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
