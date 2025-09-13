#include "control.h"

int Moto;//电机PWM变量 应是Motor的
int Position=10,Target_Position=10000; //编码器的脉冲计数
int Encoder=0,Target_Velocity=0;
	
/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
	if(moto < 0)			
	{
		GPIO_ResetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 低电平
		GPIO_SetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 高电平
	}
	else
	{
		GPIO_ResetBits(MOTOR_AIN1_GPIO, MOTOR_AIN1_PIN);	 // 低电平
		GPIO_SetBits(MOTOR_AIN2_GPIO, MOTOR_AIN2_PIN);	 // 高电平
	}
	
	TIM_SetCompare1(MOTOR_PWMA_TIMx, myabs(moto));
}



/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	int Amplitude = 8300;    //===PWM满幅是8400 限制在8300
	if(Moto < -Amplitude) 
		Moto = -Amplitude;
	
	if(Moto > Amplitude)  
		Moto = Amplitude;		
}
/**************************************************************************
函数功能：按键修改运行状态 
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)  
{	
//	int tmp,Position_Amplitude = 260; 
//	tmp = click_N_Double(10);//检测按键 
//	if(tmp==1)Target_Position += Position_Amplitude;  //单击增加位置
//	else if(tmp==2)Target_Position -= Position_Amplitude;  //双击减小位置
	
	int tmp,Velocity_Amplitude = 10; 
	tmp = click_N_Double(10);//检测按键 
	if(tmp==1)Target_Velocity += Velocity_Amplitude;  //单击增加位置
	else if(tmp==2)Target_Velocity -= Velocity_Amplitude;  //双击减小位置
	
	if(Target_Velocity > 40)
	{
		Target_Velocity = 40;
	}
	else if(Target_Velocity < -40)
	{
		Target_Velocity = -40;
	}
	
 }
/**************************************************************************
函数功能：取绝对值
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a < 0)  temp = -a;  
	else temp = a;
	return temp;
}
