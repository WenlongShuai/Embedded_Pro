#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_i2c_eeprom.h"

int main()
{
	uint8_t read = 0;
	LED_GPIO_Config();
	I2C_GPIO_Config();
	USART_GPIO_Init();
	
	I2C_EEPROM_WriteByte(0x03,0x12);
	EEPROM_WaitForWriteEnd();
	read = I2C_EEPROM_ReadRandom(0x01);
	printf("--->0x%02X",read);
	
	if(read == 0x12)
	{
		LED_G(ON);
	}
	else
	{
		LED_R(ON);
	}
	
	while(1)
	{
		
	}
	return 0;
}

