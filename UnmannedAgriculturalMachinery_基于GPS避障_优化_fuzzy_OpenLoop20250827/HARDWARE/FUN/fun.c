#include "fun.h"

//将角度限制到（0,2*PI）
float angle_format(float angle)
{
	if(angle >= 360)
	{
		return angle - 360;
	}
	
	if(angle < 0)
	{
		return angle + 360;
	}
	return angle;
}


/**************************************************************************
Function: Limiting function
Input   : Value
Output  : none
函数功能：限幅函数
入口参数：幅值
返回  值：无
**************************************************************************/
float target_limit_float(float insert,float low,float high)
{
	if (insert < low)
			return low;
	else if (insert > high)
			return high;
	else
			return insert;	
}

// 实现断言失败处理函数
void assert_failed(uint8_t* file, uint32_t line) 
{
	// 示例1：通过串口打印错误信息（需先初始化串口）
	printf("Assertion failed: file %s, line %d\r\n", file, line);
	
//	// 示例2：点亮错误指示灯
//	GPIO_SetBits(GPIOB, GPIO_Pin_0);  // 假设PB0连接了LED
//	
//	// 示例3：进入死循环，方便调试
//	while (1) {
//			// 可以添加闪烁LED等提示
//	}
}



