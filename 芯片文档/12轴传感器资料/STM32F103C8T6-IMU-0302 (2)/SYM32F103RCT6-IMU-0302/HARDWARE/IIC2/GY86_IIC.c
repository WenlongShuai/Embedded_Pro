#include "GY86_IIC.h"
#include "delay.h"
#include "stm32f10x_i2c.h"
//********接口定义*********************************************************//
//调用库函数将 I2C2端口配置好（这里使用的是 PB10、PB11 端口）：
//程序代码如下:
//************************************************************************//

static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;

void GY86_IIC_Init(void)
	{ 
		
	GPIO_InitTypeDef GPIO_InitStructure;                                              //GPIO 结构体定义		
	I2C_InitTypeDef I2C_InitStructure;                                                //I2C 结构体定义
	
  //*I2C2-模式 配置*//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);	                              //I2C2 时钟使能		
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                        //选择I2C功能
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                       //I2C应答使能
	I2C_InitStructure.I2C_ClockSpeed = 400000;                                        //时钟速率，以 HZ 为单位的，最高为 400khz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                                //该参数只有在 I2C 工作在快速模式（时钟工作频率高于 100KHz）下才有意义
	I2C_InitStructure.I2C_OwnAddress1 =0x0a;                                          //设置第一个设备自身地址
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;         //MPU6050 地址为 7 位所以设置 7 位就行了
	I2C_Init(I2C2, &I2C_InitStructure);                                               //初始化结构体配置
	I2C_Cmd(I2C2, ENABLE);                                                            //使能 I2C2
			
			//*I2C2-IO 口配置*//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);                              //使能GPIOB时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                                   //复用功能的开漏输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;                          //使能B.10和B.11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 //速度最高50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);                                            //初始化结构体配置  
	}
	
	
//************IIC单字节写函数******************************************//	
u8 GY86_Write_shortByte(u8 ADDR,u8 reg,u8 REG_data)
{   
	  delay_ms(2); 	
		I2C_GenerateSTART(I2C2, ENABLE);   
		I2CTimeout = I2CT_FLAG_TIMEOUT;	//产生起始位
	
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                      //清除 EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
			
		I2CTimeout = I2CT_FLAG_TIMEOUT;	
		I2C_Send7bitAddress(I2C2,ADDR,I2C_Direction_Transmitter);                        //发送器件地址 
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1，清除 EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
		
		I2C_SendData(I2C2,reg);                                                          //寄存器具体地址
		
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
			
		I2C_SendData(I2C2, REG_data);                                                    //发送数据
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //清除EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
			
		I2C_GenerateSTOP(I2C2, ENABLE);                                                  //产生停止信号
	return 0;
}

//************IIC多字节写函数******************************************//	
u8 G86_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf)
{   
	  delay_ms(2); 
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //产生起始位
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
		
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Transmitter);                       //发送器件地址
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))          //ADDR=1，清除 EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_SendData(I2C2, reg);                                                          //寄存器具体地址
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
		while(len--)                                                                      //利用 while 循环 发送数据
		{
			I2C_SendData(I2C2, *buf);                                                     //发送数据
			buf++;                                                                        //数据指针移位
			I2CTimeout = I2CT_FLAG_TIMEOUT;
			while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))             //清除EV8
			{
				if((I2CTimeout--) == 0) return 4;
			}
		}
		I2C_GenerateSTOP(I2C2, ENABLE);                                                   //产生停止信号
	return 0;
}



//***********IIC多字节读函数********************************************//
u8 GY86_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf)                                //需要两个起始信号

{  
   	delay_ms(2);
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))                                    //调用库函数检测 I2C 器件是否处于 BUSY 状态
		{
			if((I2CTimeout--) == 0) return 1;
		}
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //开启信号
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Transmitter);                       //写入器件地址
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))        //清除 EV6
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
			
		I2C_SendData(I2C2, reg);                                                          //发送读的地址
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                  //清除 EV8
		{
			if((I2CTimeout--) == 0) return 4;
		}
			
		I2C_GenerateSTART(I2C2, ENABLE);                                                  //开启信号
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))                       //清除 EV5
		{
			if((I2CTimeout--) == 0) return 5;
		}
			
		I2C_Send7bitAddress(I2C2, ADDR, I2C_Direction_Receiver);                          //将器件地址传出，主机为读
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))            //清除EV6
		{
			if((I2CTimeout--) == 0) return 6;
		}
			
		while(len)
		{
						if(len == 1)//只剩下最后一个数据时进入 if 语句
							{
									I2C_AcknowledgeConfig(I2C2, DISABLE);//最后有一个数据时关闭应答位
									I2C_GenerateSTOP(I2C2, ENABLE);//最后一个数据时使能停止位
							}
						if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) //读取数据
							{
									*buf = I2C_ReceiveData(I2C2);//调用库函数将数据取出到 pBuffer
									buf++; //指针移位
									len--;//字节数减 1
						 // 	delay_ms(1);
							}
				}				
		I2C_AcknowledgeConfig(I2C2, ENABLE);                                              //将应答位使能回去，等待下次通信	
return 0;				
}


//************IIC单独写命令******************************************//	
u8 GY86_Write_commendByte(u8 ADDR,u8 reg)
{   
	  delay_ms(2);                                                                     //写字节间隔
		I2C_GenerateSTART(I2C2, ENABLE);                                                 //产生起始位
		I2CTimeout = I2CT_FLAG_TIMEOUT;
	
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));                      //清除 EV5
		{
			if((I2CTimeout--) == 0) return 1;
		}
			
		I2C_Send7bitAddress(I2C2,ADDR,I2C_Direction_Transmitter);                        //发送器件地址 
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))         //ADDR=1，清除 EV6
		{
			if((I2CTimeout--) == 0) return 2;
		}
			
		I2C_SendData(I2C2,reg);                                                          //寄存器具体地址
		I2CTimeout = I2CT_FLAG_TIMEOUT;
		
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))                 //移位寄存器非空，数据寄存器已经空，产生 EV8，发送数据到 DR 既可清除该事件
		{
			if((I2CTimeout--) == 0) return 3;
		}
			
		I2C_GenerateSTOP(I2C2, ENABLE);                                                  //产生停止信号
	return 0;
}

/**************************IIC单字节单位改变********************************************
*函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*功　　能:	     读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入：	ADDR     器件地址
      reg	     寄存器地址
		  bitNum   要修改目标字节的bitNum位
		  data     为0 时，目标位将被清0 否则将被置位
		   
返回   成功为1  
 		   失败为0 
*******************************************************************************/ 
void GY86_bitchange( u8 ADDR, u8 reg, u8 bitNum, u8 data)
{
	  u8 buffer[1],b;
    GY86_Read_Byte(ADDR,reg,1,buffer);
	  b=buffer[0];
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
		GY86_Write_shortByte(ADDR, reg, b);
}

/**************************IIC单字节多位改变********************************************
*函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入;  ADDR       器件地址
       reg	      寄存器地址
		   bitStart   目标字节的起始位
	     length     位长度
		   data       存放改变目标字节位的值
		
返回   成功 为1 
 		   失败为0
*******************************************************************************/ 
u8 GY86_bitschange(u8 ADDR, u8 reg,u8 bitStart,u8 length,u8 data)
{

    u8 buffer[1],b;
    if (GY86_Read_Byte(ADDR,reg,1,buffer) == 0) {
        u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return GY86_Write_shortByte(ADDR, reg, b);
    } else {
        return 1;
    }
}
