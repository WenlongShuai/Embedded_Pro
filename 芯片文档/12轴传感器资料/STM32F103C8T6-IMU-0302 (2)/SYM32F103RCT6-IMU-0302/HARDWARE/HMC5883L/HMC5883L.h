#include "sys.h"


//定义器件在IIC总线中HMC5883L
#define	HMC5883L_Addr   0x3D	//磁场传感器器件地址

#define HMC58X3_R_CONFA 0x00  //配置寄存器A地址
#define HMC58X3_R_CONFB 0x01  //配置寄存器B地址
#define HMC58X3_R_MODE  0x02  //工作模式地址


void  Init_HMC5883L(void);
void READ_HMC5883L(void);

