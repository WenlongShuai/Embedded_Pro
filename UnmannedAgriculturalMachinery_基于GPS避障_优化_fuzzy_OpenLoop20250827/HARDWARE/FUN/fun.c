#include "fun.h"

//���Ƕ����Ƶ���0,2*PI��
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
�������ܣ��޷�����
��ڲ�������ֵ
����  ֵ����
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

// ʵ�ֶ���ʧ�ܴ�����
void assert_failed(uint8_t* file, uint32_t line) 
{
	// ʾ��1��ͨ�����ڴ�ӡ������Ϣ�����ȳ�ʼ�����ڣ�
	printf("Assertion failed: file %s, line %d\r\n", file, line);
	
//	// ʾ��2����������ָʾ��
//	GPIO_SetBits(GPIOB, GPIO_Pin_0);  // ����PB0������LED
//	
//	// ʾ��3��������ѭ�����������
//	while (1) {
//			// ���������˸LED����ʾ
//	}
}



