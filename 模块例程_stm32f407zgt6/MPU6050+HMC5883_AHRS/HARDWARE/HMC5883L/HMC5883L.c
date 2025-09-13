#include "HMC5883L.h"
#include "IMU_IIC_Hardware.h"
#include "IMU_IIC_Software.h"
#include "delay.h"
#include "sys.h"
#include "math.h"  

float  angle = 0;                                                  //航向角度
                                                   //HMC5883L三轴原始数据
//************************初始化HMC5883L**************************************//
void  Init_HMC5883L(void)
{
	#if HMC5883L_IIC_TYPE
		Software_iic_init();                                                  /* 初始化IIC接口 */
	#else
		Hardware_IIC_Init();
	#endif
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_CONFA, 0x14);	//采样平均数1 输出速率75Hz 正常测量
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_CONFB, 0x00);	//增益控制(±1.3Ga,1090高斯,-2048~2047)
	HMC5883L_write_byte(HMC5883L_Addr, HMC58X3_R_MODE, 0x00);		//连续测量模式
}


/**
 * @brief       往ATK-MS6050的指定寄存器写入一字节数据
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              dat : 写入的数据
 * @retval      ATK_MS6050_EOK : 函数执行成功
 *              ATK_MS6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t HMC5883L_write_byte(uint8_t addr, uint8_t reg, uint8_t dat)
{
	uint8_t res = 0;
	#if HMC5883L_IIC_TYPE
		res = Software_Write_longByte(addr, reg, 1, &dat);
		if(res == 0)
			return HMC58X3_EOK;
		else 
			return HMC58X3_EACK;
	#else
		res = Hardware_Write_shortByte(addr, reg, dat);
		if(res == 0)
			return HMC58X3_EOK;
		else
			return HMC58X3_EACK;
	#endif
}

//*************************读取HMC5883L数据***********************************//
void READ_HMC5883L(int16_t *mx, int16_t *my, int16_t *mz)
{
	int x,y,z;  
	u8 BUF[6],flag[1];                             //接收数据缓存区

	#if HMC5883L_IIC_TYPE
		Software_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //这一位实时清0否则无法工作
		Software_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //查看HMC5883L数据存放状态
	#else
		Hardware_bitchange(HMC5883L_Addr,HMC58X3_R_CONFA,7,0);         //这一位实时清0否则无法工作
		Hardware_Read_Byte(HMC5883L_Addr,0x09,1,flag);                 //查看HMC5883L数据存放状态
	#endif
	
	if(flag[0]&0x01)                                           //判断HMC5883L数据是否存放完成
	{
		#if HMC5883L_IIC_TYPE
			Software_Read_Byte(HMC5883L_Addr,0x03,6,BUF);
		#else
			Hardware_Read_Byte(HMC5883L_Addr,0x03,6,BUF);                //读取HMC5883L的X,Y,Z轴数据（OUT_X_L_A>OUT_X_H_A>OUT_Z_L_A>OUT_Z_H_A>OUT_Y_L_A>OUT_Y_H_A）
		#endif
		x=(BUF[0] << 8) | BUF[1];                                //合并X轴寄存器输出数据的高位与低位
		z=(BUF[2] << 8) | BUF[3];                                //合并Z轴寄存器输出数据的高位与低位
		y=(BUF[4] << 8) | BUF[5];                                //合并Y轴寄存器输出数据的高位与低位
		if(x>0x7fff)x-=0xffff;	                                 //X轴数据限幅
		if(z>0x7fff)z-=0xffff;	                                 //Z轴数据限幅
		if(y>0x7fff)y-=0xffff;	                                 //Y轴数据限幅
		
		*mx = x;
		*my = y;
		*mz = z;
//		*angle= atan2(x,y) * (180 / 3.14159265) + 180;            //换算成角度
	}
}
