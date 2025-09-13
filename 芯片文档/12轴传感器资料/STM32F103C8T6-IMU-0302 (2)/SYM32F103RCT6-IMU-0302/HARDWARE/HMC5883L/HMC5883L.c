#include "HMC5883L.h"
#include "GY86_IIC.h"
#include "delay.h"
#include "sys.h"
#include "math.h"  

float  angle;                                                  //航向角度
                                                   //HMC5883L三轴原始数据
//************************初始化HMC5883L**************************************//
void  Init_HMC5883L(void)
{

	GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_CONFA,0x14);    //采样平均数1 输出速率75Hz 正常测量
  GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_CONFB,0x00);    //增益控制(±1.3Ga,1090高斯,-2048~2047)
  GY86_Write_shortByte(HMC5883L_Addr,HMC58X3_R_MODE,0x00);     //连续测量模式
 
}

//*************************读取HMC5883L数据***********************************//
void READ_HMC5883L(void)
{
	   int x,y,z;  
	   u8 BUF[6],flag[1];                             //接收数据缓存区
	    
	  GY86_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //这一位实时清0否则无法工作
    GY86_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //查看HMC5883L数据存放状态
		if(flag[0]&0x01)                                           //判断HMC5883L数据是否存放完成
		{
			GY86_Read_Byte(HMC5883L_Addr,0x03,6,BUF);                //读取HMC5883L的X,Y,Z轴数据（OUT_X_L_A>OUT_X_H_A>OUT_Z_L_A>OUT_Z_H_A>OUT_Y_L_A>OUT_Y_H_A）
			x=(BUF[0] << 8) | BUF[1];                                //合并X轴寄存器输出数据的高位与低位
			z=(BUF[2] << 8) | BUF[3];                                //合并Z轴寄存器输出数据的高位与低位
      y=(BUF[4] << 8) | BUF[5];                                //合并Y轴寄存器输出数据的高位与低位
			if(x>0x7fff)x-=0xffff;	                                 //X轴数据限幅
			if(z>0x7fff)z-=0xffff;	                                 //Z轴数据限幅
			if(y>0x7fff)y-=0xffff;	                                 //Y轴数据限幅
			angle= atan2(x,y) * (180 / 3.14159265) + 180;            //换算成角度
			
		}
}
