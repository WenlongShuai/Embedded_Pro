#include "stm32f10x.h"
#include "bsp_fullcolor_led.h"
#include "bsp_usart.h"

/* 
stm32 使用SRAM调试代码
主要步骤：
1、在原工程基础上创建一个调试版本，只需要创建一个执行文件，其他文件跟正常版本保持一致。
2、修改分散加载文件，使链接器把代码分配到内部SRAM空间。
3、添加宏修改STM32的向量表地址。
4、修改仿真器和下载器的配置，使程序能够通过下载器存储在SRAM空间。
5、根据使用情况选择是否需要使用仿真器命令脚本文件*.ini。
6、尝试给SRAM下载程序或者仿真调试。
*/

void delay(uint32_t time);

int main()
{
	USART_GPIO_Init();
	FullColor_LED_Init();
	
	while(1)
	{
		setRGBColor(0x805009);
		delay(0x5FFFFF);
		
		setRGBColor(0x20FFF0);
		delay(0x5FFFFF);
		
		setRGBColor(0xAAC0FF);
		delay(0x5FFFFF);
	}
	return 0;
}


void delay(uint32_t time)
{
	while(time--);
}



