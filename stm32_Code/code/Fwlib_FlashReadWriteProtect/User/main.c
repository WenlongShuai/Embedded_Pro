#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_readWriteInterFlash.h"

/* stm32进行内部FLASH读写保护实验 */

int main(void)
{ 	
	  /*初始化USART，配置模式为 115200 8-N-1*/
  USART_GPIO_Init();
	LED_GPIO_Config();
	Key_GPIO_Config();

	LED_G(ON);
	
	//芯片自动复位后，串口可能有小部分异常输出，如输出一个“？”号
	printf("\r\n\n\n欢迎使用野火  STM32  开发板。\r\n");	
	printf("这是读写保护测试实验\r\n");
	
	/* 获取写保护寄存器的值进行判断，寄存器位为0表示有保护，为1表示无保护 */
	/*  若不等于0xFFFFFFFF，则说明有部分页被写保护了 */
	if(FLASH_GetWriteProtectionOptionByte() !=0xFFFFFFFF )
	{
		printf("\r\n目前芯片处于写保护状态，按Key1键解除保护\r\n");
		printf("写保护寄存器的值：WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	else //无写保护
	{
		printf("\r\n目前芯片无 写 保护，按 Key1 键可设置成 写 保护\r\n");
		printf("写保护寄存器的值：WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	
	
	/*  若等于SET，说明处于读保护状态 */
	if(FLASH_GetReadOutProtectionStatus () == SET )
	{
		printf("\r\n目前芯片处于读保护状态，按Key2键解除保护\r\n");
	}
	else
	{
		printf("\r\n目前芯片无 读 保护，按 Key2 键可设置成 读 保护\r\n");
	}
	
	LED_G(OFF);
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			LED_TOGGLE(R);
			WriteProtect_Toggle();
		} 

		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			LED_TOGGLE(B)
			ReadProtect_Toggle();			
		}		
	}	
	return 0;
}