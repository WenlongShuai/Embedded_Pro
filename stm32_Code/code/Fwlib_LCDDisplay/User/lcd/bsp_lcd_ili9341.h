#ifndef __BSP_LCD_ILI9341_H__
#define __BSP_LCD_ILI9341_H__


#include "stm32f10x.h"

#define FSMC_Addr_ILI9341_DATA   ((__IO uint16_t *)(0x60000000 | (1<<(16+1))))
#define FSMC_Addr_ILI9341_CMD    ((__IO uint16_t *)(0x60000000 & ~(1<<(16+1))))

#define RGB888_CONVERT_RGB565(R,G,B)   ((B&0x1F)|((G&0x3F)<<5)|((R&0x1F)<<11))  


/* 定义 LCD 驱动芯片 ID */
#define     LCDID_UNKNOWN             0
#define     LCDID_ILI9341             0x9341
#define     LCDID_ST7789V             0x8552

/******************************* 定义 ILI934 常用命令 ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		        0x2C	     //填充像素


#define LCD_GPIO_CLK RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
#define LCD_FSMC_CLK    RCC_AHBPeriph_FSMC
#define LCD_FSMC_BANK_NORSRAMx  FSMC_Bank1_NORSRAM1

/* 液晶数据接口 */
#define LCD_DB00_PIN   GPIO_Pin_14
#define LCD_DB00_PORT  GPIOD
#define LCD_DB01_PIN   GPIO_Pin_15
#define LCD_DB01_PORT  GPIOD
#define LCD_DB02_PIN   GPIO_Pin_0
#define LCD_DB02_PORT  GPIOD
#define LCD_DB03_PIN   GPIO_Pin_1
#define LCD_DB03_PORT  GPIOD
#define LCD_DB04_PIN   GPIO_Pin_7
#define LCD_DB04_PORT  GPIOE
#define LCD_DB05_PIN   GPIO_Pin_8
#define LCD_DB05_PORT  GPIOE
#define LCD_DB06_PIN   GPIO_Pin_9
#define LCD_DB06_PORT  GPIOE
#define LCD_DB07_PIN   GPIO_Pin_10
#define LCD_DB07_PORT  GPIOE
#define LCD_DB08_PIN   GPIO_Pin_11
#define LCD_DB08_PORT  GPIOE
#define LCD_DB09_PIN   GPIO_Pin_12
#define LCD_DB09_PORT  GPIOE
#define LCD_DB10_PIN   GPIO_Pin_13
#define LCD_DB10_PORT  GPIOE
#define LCD_DB11_PIN   GPIO_Pin_14
#define LCD_DB11_PORT  GPIOE
#define LCD_DB12_PIN   GPIO_Pin_15
#define LCD_DB12_PORT  GPIOE
#define LCD_DB13_PIN   GPIO_Pin_8
#define LCD_DB13_PORT  GPIOD
#define LCD_DB14_PIN   GPIO_Pin_9
#define LCD_DB14_PORT  GPIOD
#define LCD_DB15_PIN   GPIO_Pin_10
#define LCD_DB15_PORT  GPIOD


/* 液晶控制接口 */
#define LCD_RST_PIN   GPIO_Pin_1
#define LCD_RST_PORT  GPIOE
#define LCD_BL_PIN   GPIO_Pin_12
#define LCD_BL_PORT  GPIOD
#define LCD_CS_PIN   GPIO_Pin_7
#define LCD_CS_PORT  GPIOD
#define LCD_RD_PIN   GPIO_Pin_4
#define LCD_RD_PORT  GPIOD
#define LCD_WE_PIN   GPIO_Pin_5
#define LCD_WE_PORT  GPIOD
#define LCD_RS_PIN   GPIO_Pin_11
#define LCD_RS_PORT  GPIOD


/* 液晶触摸接口 */
#define LCD_TP_DCLK_PIN   GPIO_Pin_1
#define LCD_TP_DCLK_PORT  GPIOE
#define LCD_TP_DOUT_PIN   GPIO_Pin_3
#define LCD_TP_DOUT_PORT  GPIOE
#define LCD_TP_DIN_PIN   GPIO_Pin_2
#define LCD_TP_DIN_PORT  GPIOE
#define LCD_TP_CS_PIN   GPIO_Pin_13
#define LCD_TP_CS_PORT  GPIOD
#define LCD_TP_PENIRQ_PIN   GPIO_Pin_4
#define LCD_TP_PENIRQ_PORT  GPIOE


void LCD_FSMC_Init(void);
void ILI9341_Rst(void);
void ILI9341_Write_Cmd ( uint16_t usCmd );
void ILI9341_Write_Data ( uint16_t usData );
uint16_t ILI9341_Read_Data ( void );
static void ILI9341_Delay ( __IO uint32_t nCount );
void ILI9341_BackLed_Control( FunctionalState enumState );


uint16_t ILI9341_ReadPixelFormat(void);
void drawPoint(uint16_t x, uint16_t y, uint16_t color);
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);


#endif /* __BSP_LCD_ILI9341_H__ */
