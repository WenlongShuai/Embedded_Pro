#include <stdio.h>
#include "oled.h"
#include "key.h"
#include "usart.h"
#include "delay.h"


extern unsigned char BMP1[];
extern unsigned char BMP2[];
extern unsigned char BMP3[];
extern unsigned char electricQuantityIco0[];
extern unsigned char electricQuantityIco1[];
extern unsigned char electricQuantityIco2[];
extern unsigned char electricQuantityIco3[];

extern unsigned char bluetoothIcon[];
extern unsigned char wifiIcon[];

extern Key_t key0;
extern uint8_t key_Process_Time;
extern OLED_MENU_STRUCT oled_menu_struct;


void oledDataDis(int leftActualSpeed, int leftCurrentSpeed, float leftMileage, int rightActualSpeed, int rightCurrentSpeed, float rightMileage, float FW, float FY, float HX, int Mode)
{
	uint8_t disBuff[128] = {0};
	sprintf((char *)disBuff, "L(m/s) as:%d",leftActualSpeed);
	OLED_ShowStr(0, 0, disBuff, 1);
	sprintf((char *)disBuff, "  cs:%d   cg:%.2f",leftCurrentSpeed, leftMileage);
	OLED_ShowStr(0, 1, disBuff, 1);

	sprintf((char *)disBuff, "R(m/s) as:%d",rightActualSpeed);
	OLED_ShowStr(0, 2, disBuff, 1);
	sprintf((char *)disBuff, "  cs:%d   cg:%.2f",rightCurrentSpeed, rightMileage);
	OLED_ShowStr(0, 3, disBuff, 1);
	
	sprintf((char *)disBuff, "IMU fw:%.2f",FW);
	OLED_ShowStr(0, 4, disBuff, 1);
	sprintf((char *)disBuff, "  fy:%.2f",FY);
	OLED_ShowStr(0, 5, disBuff, 1);
	sprintf((char *)disBuff, "  hx:%.2f",HX);
	OLED_ShowStr(0, 6, disBuff, 1);
	
	sprintf((char *)disBuff, "mode:%d",Mode);
	OLED_ShowStr(0, 7, disBuff, 1);
}


int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	SysTick_Init();
	uart_init(115200);
	delay_init(168);
	
	#ifdef SOFTWARE_IIC  //软件I2C
	IICx_Init();
	#else  //硬件I2C
	I2C_Mode_Config();
	#endif
	
	delay_us(500);
	
	OLED_Init();
	
	while(1)
	{
//		oledDataDis(100,101,102,200,201,202,20,30,40,1);
		OLED_ShowStr(0, 3, "hhhhh", 2);

		delay_us(100);
	}
	
	return 0;
}





