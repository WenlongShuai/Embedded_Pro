#ifndef __SPI_H__
#define __SPI_H__

#include "sys.h"

//-----------------------------SPI 总线配置--------------------------------------//
#define USE_HARDWARE_SPI     0  //1:Enable Hardware SPI;0:USE Soft SPI


//本测试程序使用的是模拟SPI接口驱动
//可自由更改接口IO配置，使用任意最少4 IO即可完成本款液晶驱动显示
/******************************************************************************
接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10连接至TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 连接至TFT --CS
#define LCD_RST     	  GPIO_Pin_12	//PB12连接至TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13连接至TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15连接至TFT - SDI
*******************************************************************************/
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //MCU_PB9--->>TFT --BL
#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_Pin_11 //MCU_PB11--->>TFT --CS/CE
#define LCD_RST     		GPIO_Pin_12	//PB10--->>TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN


//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define	LCD_CS_SET  	LCD_CTRL->BSRRL|=LCD_CS    
#define	LCD_RS_SET  	LCD_CTRL->BSRRL|=LCD_RS    
#define	LCD_SDA_SET  	LCD_CTRL->BSRRL|=LCD_SDA    
#define	LCD_SCL_SET  	LCD_CTRL->BSRRL|=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRRL|=LCD_RST    
#define	LCD_LED_SET  	LCD_CTRL->BSRRL|=LCD_LED   

//液晶控制口置0操作语句宏定义
#define	LCD_CS_CLR  	LCD_CTRL->BSRRH|=LCD_CS    
#define	LCD_RS_CLR  	LCD_CTRL->BSRRH|=LCD_RS    
#define	LCD_SDA_CLR  	LCD_CTRL->BSRRH|=LCD_SDA    
#define	LCD_SCL_CLR  	LCD_CTRL->BSRRH|=LCD_SCL    
#define	LCD_RST_CLR  	LCD_CTRL->BSRRH|=LCD_RST    
#define	LCD_LED_CLR  	LCD_CTRL->BSRRH|=LCD_LED 


#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //数据输出
#define LCD_DATAIN     LCD_DATA->IDR;   //数据输入

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

void LCD_GPIO_Init(void);
void  SPIv_WriteData(uint8_t Data);
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte);
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet);
void SPI2_Init(void);

#endif /* __SPI_H__ */
