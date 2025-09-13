#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "HC_SR04.h"


extern u8  TIMxCHx_CAPTURE_STA;	//输入捕获状态		    				
extern u32	TIMxCHx_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
extern float distance;

int main(void)
{ 
	u32 time=0; 
	SysTick_Init();
	uart_init(115200);
	Hcsr04Init();
	
	printf("hc-sr04 start!\r\n");
	while(1)
	{
		Hcsr04Start();//  开启超声波模块测距
		
		if(TIMxCHx_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			time=TIMxCHx_CAPTURE_STA&0X3F;
			time *= 65535;//溢出时间总和
			time+=TIMxCHx_CAPTURE_VAL;//得到总的高电平时间
			distance = time*0.033/2;
			printf("----> %.2f\r\n",distance);
			TIMxCHx_CAPTURE_STA=0;//开启下一次捕获
		}		
		Delay_ms(70);
	}
}
