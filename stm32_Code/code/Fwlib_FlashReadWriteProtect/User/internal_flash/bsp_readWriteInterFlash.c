#include "bsp_readWriteInterFlash.h"


void write_Protect_Enable(void)
{
	FLASH_Unlock();
	FLASH_EraseOptionBytes();
	FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);  //需要保护的页
	NVIC_SystemReset();  //写保护的时候，需要进行系统，来重装载选择字节
}

void write_Protect_Disable(void)
{
	FLASH_Unlock();
	FLASH_EraseOptionBytes();
	FLASH_EnableWriteProtection(0x00000000);  //需要解除保护的页
	NVIC_SystemReset();  //写保护的时候，需要进行系统，来重装载选择字节
}


void read_Protect_Enable(void)
{
	FLASH_Unlock();
	FLASH_ReadOutProtection(ENABLE);
	/* 设置读保护选项字节之后，需要上电复位使选项字节生效 */
}

void read_Protect_Disable(void)
{
	FLASH_Unlock();
	FLASH_ReadOutProtection(DISABLE);
	/* 设置读保护选项字节之后，需要上电复位使选项字节生效 */
}

void WriteProtect_Toggle(void)
{
	if(FLASH_GetWriteProtectionOptionByte() !=0xFFFFFFFF)
	{
		printf("\r\n芯片处于写保护状态，即将进行解除保护...\r\n");
		FLASH_Unlock();
		
		FLASH_EraseOptionBytes();
		
		FLASH_EnableWriteProtection(0x00000000);  //需要解除保护的页
		
		printf("\r\n芯片自动复位加载新配置，复位后芯片就处于无写保护状态\r\n");
		
		NVIC_SystemReset();  //写保护的时候，需要进行系统，来重装载选择字节
	}
	else
	{
		printf("\r\n芯片处于无写保护状态，即将进行写保护...\r\n");
		
		FLASH_Unlock();
		
		FLASH_EraseOptionBytes();
		
		FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);  //需要保护的页
		
		printf("\r\n芯片自动复位加载新配置，复位后芯片就处于写保护状态\r\n");
		
		NVIC_SystemReset();  //写保护的时候，需要进行系统，来重装载选择字节
	}
}

void ReadProtect_Toggle(void)
{
	if(FLASH_GetReadOutProtectionStatus () == SET)
	{
		printf("芯片处于读保护状态，即将进行解除读保护状态...\r\n");
		
		FLASH_Unlock();
		
		printf("即将解除读保护，解除读保护会把FLASH的所有内容清空\r\n");
		printf("由于解除后程序被清空，所以后面不会有任何提示输出\r\n");
		printf("等待20秒后即可给芯片下载新的程序...\r\n");
		
		FLASH_ReadOutProtection(DISABLE);
	}
	else
	{
		printf("\r\n芯片处于无读保护状态，即将进行读保护...\r\n");
		
		FLASH_Unlock();
		
		FLASH_ReadOutProtection(ENABLE);
		
		printf("\r\n芯片已被设置为读保护，上电复位后生效（必须重新给开发板上电，只按复位键无效）\r\n");
		printf("\r\n处于保护状态下无法正常下载新程序，必须要先解除保护状态再下载\r\n");
	}
}

