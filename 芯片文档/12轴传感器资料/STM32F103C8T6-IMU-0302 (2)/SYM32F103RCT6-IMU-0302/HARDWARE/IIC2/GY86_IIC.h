#include "sys.h"

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

void GY86_IIC_Init(void);
u8 GY86_Write_shortByte(u8 ADDR ,u8 reg,u8 REG_data);
u8 G86_Write_longByte(u8 ADDR,u8 reg,u8 len,u8 *buf);
u8 GY86_Read_Byte(u8 ADDR,u8 reg,u8 len,u8 *buf);
void GY86_bitchange(u8 ADDR, u8 reg, u8 bitNum, u8 data);
u8 GY86_bitschange(u8 ADDR, u8 reg,u8 bitStart,u8 length,u8 data);
u8 GY86_Write_commendByte(u8 ADDR,u8 reg);
