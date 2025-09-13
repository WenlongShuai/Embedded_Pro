#include "sys.h"

//IIC类型，0：硬件IIC 1：软件IIC
#define HMC5883L_IIC_TYPE   1

//定义器件在IIC总线中HMC5883L
//0x1E地址是未加上读写位  读：(0x1E<<1)|1   写：(0x1E<<1)|0
#define	HMC5883L_Addr   0x1E	//磁场传感器器件地址

#define HMC58X3_R_CONFA 0x00  //配置寄存器A地址
#define HMC58X3_R_CONFB 0x01  //配置寄存器B地址
#define HMC58X3_R_MODE  0x02  //工作模式地址

/* 函数错误代码 */
#define HMC58X3_EOK      0   /* 没有错误 */
#define HMC58X3_EID      1   /* ID错误 */
#define HMC58X3_EACK     2   /* IIC通讯ACK错误 */


void  Init_HMC5883L(void);
void READ_HMC5883L(void);
uint8_t HMC5883L_write_byte(uint8_t addr, uint8_t reg, uint8_t dat);

