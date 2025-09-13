#ifndef __SOFTWARE_IIC_H__
#define __SOFTWARE_IIC_H__

#include "sys.h"


//IO配置
#define IIC2_SDA_GPIO_RCC   	RCC_AHB1Periph_GPIOC
#define IIC2_SDA_GPIO_PORT   	GPIOC
#define IIC2_SDA_GPIO_PIN   	GPIO_Pin_9

#define IIC2_SCL_GPIO_RCC   	RCC_AHB1Periph_GPIOA
#define IIC2_SCL_GPIO_PORT   	GPIOA
#define IIC2_SCL_GPIO_PIN   	GPIO_Pin_8

//IO方向设置
#define SDA2_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PF0输入模式
#define SDA2_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} 	//PF0输出模式
//IO操作函数	 
#define IIC2_SCL    PAout(8) //SCL
#define IIC2_SDA    PCout(9) //SDA	 
#define READ2_SDA   PCin(9)  //输入SDA 

//IIC所有操作函数
void IICx_Init(void);
void IIC2_Start(void);				//发送IIC开始信号
void IIC2_Stop(void);	  			//发送IIC停止信号
void IIC2_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC2_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC2_Wait_Ack(void); 				//IIC等待ACK信号
void IIC2_Ack(void);					//IIC发送ACK信号
void IIC2_NAck(void);				//IIC不发送ACK信号

#endif /* __IIC_H__ */
