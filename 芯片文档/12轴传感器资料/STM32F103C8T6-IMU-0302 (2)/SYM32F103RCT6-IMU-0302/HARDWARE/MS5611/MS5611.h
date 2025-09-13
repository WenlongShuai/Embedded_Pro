#include "sys.h"

#define  MS561101BA_Addr                0xEE          //器件地址
#define  MS561101BA_RST                 0x1E          //复位指令
#define  MS561101BA_PROM_RD             0xA0          //PROM起始地址(出厂校准数据存放的位置)
#define  MS561101BA_PROM_CRC            0xAE          //校验指令存放的地址

//**********************ADC通道开启转换地址***************************//
#define  MS561101BA_OSR_256             0x00          //8位分辨率转换控制地址
#define  MS561101BA_OSR_512             0x02          //9位分辨率转换控制地址
#define  MS561101BA_OSR_1024            0x04          //10位分辨率转换控制地址
#define  MS561101BA_OSR_2048            0x06          //11位分辨率转换控制地址
#define  MS561101BA_OSR_4096            0x08          //12位分辨率转换控制地址
//**********************数字压力寄存器地址****************************//
#define  MS561101BA_D1_OSR_256          0x40          //8位分辨率地址
#define  MS561101BA_D1_OSR_512          0x42          //9位分辨率地址
#define  MS561101BA_D1_OSR_1024         0x44          //10位分辨率地址
#define  MS561101BA_D1_OSR_2048         0x46          //11位分辨率地址
#define  MS561101BA_D1_OSR_4096         0x48          //12位分辨率地址
//**********************数字温度寄存器地址****************************//
#define  MS561101BA_D2_OSR_256          0x50          //8位分辨率地址
#define  MS561101BA_D2_OSR_512          0x52          //9位分辨率地址
#define  MS561101BA_D2_OSR_1024         0x54          //10位分辨率地址
#define  MS561101BA_D2_OSR_2048         0x56          //11位分辨率地址
#define  MS561101BA_D2_OSR_4096         0x58          //12位分辨率地址



void Init_MS561101BA(void);
void MS561101BA_GetTemperature(u8 OSR_Temp);
uint32_t MS561101BA_DO_CONVERSION(uint8_t command);
u8 MS561101BA_getPressure(u8 OSR_Temp);
