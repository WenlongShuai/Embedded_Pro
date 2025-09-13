#include "stm32f10x.h"
#include "bsp_i2c_eeprom.h"
#include "bsp_led.h"
#include "bsp_usart.h"

#define ARRAYSIZE   			 10
#define EEP_Firstpage      0x00

uint8_t I2c_Buf_Write[256];
uint8_t I2c_Buf_Read[256];

uint8_t I2C_Test(void);

int main()
{
//	uint8_t read, i = 0;
//	uint8_t readArray[ARRAYSIZE] = {0};
//	uint8_t writeArray[ARRAYSIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9 ,10};
	
	LED_GPIO_Config();
	I2C_EEPROM_Init();
	USART_GPIO_Init();
	
	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");
	
	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");	
	
	//EEPROM 读写测试
	if(I2C_Test() ==1)
	{
			LED_G(ON);
	}
	else
	{
			LED_R(ON);
	}
	
	#if 0
		I2C_EEPROM_WriteByte(0x01, 0x12);
		EEPROM_WaitForWriteEnd();
		//read = I2C_EEPROM_ReadCurrent();
		read = I2C_EEPROM_ReadRandom(0x01);
		//Usart_SendByteData(USARTx, read);
		if(read == 0x12)
		{
			LED_G(ON);
		}
		else
		{
			LED_R(ON);
		}
	#elif 0
		//I2C_EEPROM_WritePage(0x00, writeArray, EEPROM_BYTE_ALIG);
		I2C_EEPROM_WriteData(0x00, writeArray, ARRAYSIZE);
		EEPROM_WaitForWriteEnd();
		I2C_EEPROM_ReadSequential(0x00, readArray, ARRAYSIZE);
		for ( i=0; i < ARRAYSIZE; i++ ) //填充缓冲
		{ 
			printf("0x%02X ", readArray[i]); 
		}
	#endif

	
	
	while(1)
	{
		
	}
	return 0;
}

uint8_t I2C_Test(void)
{
	uint16_t i;

	printf("写入的数据\n\r");
    
	for ( i=0; i<=255; i++ ) //填充缓冲
  {   
    I2c_Buf_Write[i] = i;

    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n\r");    
   }

  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	I2C_EEPROM_WriteData(EEP_Firstpage, I2c_Buf_Write, 256);
  EEPROM_WaitForWriteEnd();
  EEPROM_INFO("\n\r写成功\n\r");
   
   EEPROM_INFO("\n\r读出的数据\n\r");
  //将EEPROM读出数据顺序保持到I2c_Buf_Read中
	I2C_EEPROM_ReadSequential(EEP_Firstpage, I2c_Buf_Read, 256); 
   
  //将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致\n\r");
			return 0;
		}
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n\r");
    
	}
  EEPROM_INFO("I2C(AT24C02)读写测试成功\n\r");
  
  return 1;
}

