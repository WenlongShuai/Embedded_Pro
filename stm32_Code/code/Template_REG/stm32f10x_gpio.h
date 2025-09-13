#ifndef __STM32F10X_GPIO_H__
#define __STM32F10X_GPIO_H__

#include "stm32f10x.h"

#define GPIO_Pin_0 		(unsigned short)0x0001
#define GPIO_Pin_1 		(unsigned short)0x0002
#define GPIO_Pin_2 		(unsigned short)0x0004
#define GPIO_Pin_3 		(unsigned short)0x0008
#define GPIO_Pin_4 		(unsigned short)0x0010
#define GPIO_Pin_5 		(unsigned short)0x0020
#define GPIO_Pin_6 		(unsigned short)0x0040
#define GPIO_Pin_7 		(unsigned short)0x0080
#define GPIO_Pin_8 		(unsigned short)0x0100
#define GPIO_Pin_9 		(unsigned short)0x0200
#define GPIO_Pin_10 	(unsigned short)0x0400
#define GPIO_Pin_11 	(unsigned short)0x0800
#define GPIO_Pin_12 	(unsigned short)0x1000
#define GPIO_Pin_13 	(unsigned short)0x2000
#define GPIO_Pin_14 	(unsigned short)0x4000
#define GPIO_Pin_15 	(unsigned short)0x8000
#define GPIO_Pin_All  (unsigned short)0xFFFF

typedef enum 
{
	GPIO_Speed_10MHZ = 1,
	GPIO_Speed_2MHZ,
	GPIO_Speed_50MHZ,
}GPIO_Speed_Typedef;

typedef enum 
{
	GPIO_Mode_AIN = 0x00,    //模拟输入
	GPIO_Mode_IN_FLOATING = 0x04,  //浮空输入
	GPIO_Mode_IPD = 0x28,   //上拉输入
	GPIO_Mode_IPU = 0x48,   //下拉输入
	
	GPIO_Mode_Out_OD = 0x14,//开漏输出
	GPIO_Mode_Out_PP = 0x10,//推挽输出
	GPIO_Mode_AF_OD = 0x1C, //复用开漏输出
	GPIO_Mode_AF_PP = 0x18, //复用推挽输出
}GPIO_Mode_Typedef;

typedef struct
{
	unsigned short GPIO_Pinx;
	GPIO_Mode_Typedef GPIO_Mode;
	GPIO_Speed_Typedef GPIO_Speed;
}GPIO_InitStructure_Typedef;

void GPIO_SetBits(GPIO_Typedef *GPIOx, unsigned short GPIO_Pin);
void GPIO_ResetBits(GPIO_Typedef *GPIOx, unsigned short GPIO_Pin);
void GPIO_Init(GPIO_Typedef *GPIOx, GPIO_InitStructure_Typedef *GPIO_Struct);

#endif /*__STM32F10X_GPIO_H__*/
