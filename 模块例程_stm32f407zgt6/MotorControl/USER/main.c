#include <stdio.h>
#include "usart.h"
#include "systick.h"
#include "pwm.h"
#include "encoder.h"
#include "timer.h"
 
extern int Moto;//电机PWM变量 应是Motor的
extern int Position,Target_Position; //编码器的脉冲计数

extern int Encoder,Target_Velocity;

int main()
{
	SysTick_Init();
	uart_init(115200);
	
//	Motor_Ctrl_Init();
	
//	KEY_Init();
	
	Encoder_Init_TIM(0xffff, 3);
	
//	Timerx_Init(500-1, 16800-1);
	
//	Remote_Ctrl(2);
	
//	TIM_SetCompare1(MOTOR_PWMA_TIMx, 8300);
	
	int temp = 0;
	
	while(1)
	{
//		printf("Position: %d  ",Position);		//当前编码器位置打印到串口
//		printf("Moto: %d  ",Moto);				//当前的PID控制器输出打印到串口
//		printf("Target: %d\r\n",Target_Position);//当前的位置目标值打印到串口
//		Delay_ms(1000);
		
//		printf("%d\t%d\t%d\r\n",Position,Moto,Target_Position);		//当前编码器位置打印到串口
//		Delay_ms(10);
		
//			printf("Velocity: %d  ",Encoder);		//当前编码器位置打印到串口
//			printf("Moto: %d  ",Moto);				//当前的PID控制器输出打印到串口
//			printf("Target: %d\r\n",Target_Velocity);//当前的位置目标值打印到串口
//			Delay_ms(1000);
		
//		printf("%d\t%d\t%d\r\n",Encoder,Moto,Target_Velocity);		//当前编码器位置打印到串口
		temp = Read_Encoder_TIM();
		printf("%d\r\n",temp);
		Delay_ms(500);
	}
	
	return 0;
}

