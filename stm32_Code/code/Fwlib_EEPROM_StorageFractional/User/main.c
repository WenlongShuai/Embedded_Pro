#include "stm32f10x.h"
#include "bsp_i2c_eeprom.h"
#include "bsp_led.h"
#include "bsp_usart.h"

#define ARRAYSIZE   			 10
#define EEP_Firstpage_int      10
#define EEP_Firstpage_double   60
#define EEP_Firstpage_string   150

int intWriteBuffer[ARRAYSIZE] = {0};
double doubleWriteBuffer[ARRAYSIZE] = {0};

int intReadBuffer[ARRAYSIZE] = {0};
double doubleReadBuffer[ARRAYSIZE] = {0};

uint8_t *stringWrite = "EEPROM存储字符串实验!";
uint8_t stringRead[50] = {0};

void I2C_StorageFractional(void);
uint8_t bufferCompare(uint8_t *buffer1, uint8_t *buffer2, uint8_t len);

int main()
{	
	LED_GPIO_Config();
	I2C_EEPROM_Init();
	USART_GPIO_Init();
	
//	printf("\r\n 这是一个I2C外设(AT24C02)存储整数和小数的测试例程 \r\n");
//	I2C_StorageFractional();
	
	printf("\r\n 这是一个I2C外设(AT24C02)存储字符串的测试例程 \r\n");
	I2C_StorageString();

	while(1)
	{
		
	}
	return 0;
}

void I2C_StorageFractional(void)
{
	uint16_t i;
	uint8_t intFlag = 0;
	uint8_t doubleFlag = 0;
	

	EEPROM_INFO("\r\n写入的数据\n\r");
    
	for ( i=0; i < ARRAYSIZE; i++ ) //填充缓冲
  {   
    intWriteBuffer[i] = 100+i;
		doubleWriteBuffer[i] = 200+i+0.123456;

    printf("整数为：%d\r\n ", intWriteBuffer[i]);
    printf("小数为：%lf\r\n ", doubleWriteBuffer[i]);
   }

  //将整数的数据写入EERPOM中 
	I2C_EEPROM_WriteData(EEP_Firstpage_int, (void*)intWriteBuffer, sizeof(intWriteBuffer));
  EEPROM_WaitForWriteEnd();
	 
	//将小数的数据写入EERPOM中 
	I2C_EEPROM_WriteData(EEP_Firstpage_double, (void*)doubleWriteBuffer, sizeof(doubleWriteBuffer));
  EEPROM_WaitForWriteEnd();
	 
  EEPROM_INFO("\n\r写成功\n\r");
   
  EEPROM_INFO("\n\r读出的数据\n\r");
	 
  //将EEPROM读出的整数数据保存到intReadBuffer中
	I2C_EEPROM_ReadSequential(EEP_Firstpage_int, intReadBuffer, sizeof(intReadBuffer)); 
	 
	//将EEPROM读出的小数数据保存到doubleReadBuffer中
	I2C_EEPROM_ReadSequential(EEP_Firstpage_double, doubleReadBuffer, sizeof(doubleReadBuffer)); 
   
	intFlag = bufferCompare((void *)intWriteBuffer,(void *)intReadBuffer,sizeof(intWriteBuffer));
	doubleFlag = bufferCompare((void *)doubleWriteBuffer,(void *)doubleReadBuffer,sizeof(doubleWriteBuffer));
	 
	if(intFlag == 0 && doubleFlag == 0)
	{
		//将intReadBuffer和doubleReadBuffer中的数据通过串口打印
		for (i=0; i<ARRAYSIZE; i++)
		{	
			printf("整数为：%d\r\n ", intReadBuffer[i]);
			printf("小数为：%lf\r\n ", doubleReadBuffer[i]);
		}
		LED_G(ON);
		EEPROM_INFO("I2C(AT24C02)存储整数小数测试成功\n\r");
	}
	else
	{
		LED_R(ON);
		EEPROM_INFO("I2C(AT24C02)存储整数小数测试失败\n\r");
	}
}


void I2C_StorageString(void)
{
	uint16_t i;
	uint8_t strFlag = 0;
	

	EEPROM_INFO("\r\n写入的数据\n\r");
    
	printf("%s\n\r",stringWrite);

  //将字符串写入EERPOM中 
	I2C_EEPROM_WriteData(EEP_Firstpage_string, stringWrite, strlen(stringWrite));
  EEPROM_WaitForWriteEnd();
	
  EEPROM_INFO("\n\r写成功\n\r");
   
  EEPROM_INFO("\n\r读出的数据\n\r");
	 
  //将EEPROM读出的字符串保存到stringRead中
	I2C_EEPROM_ReadSequential(EEP_Firstpage_string, stringRead, strlen(stringWrite)); 
	  
	strFlag = bufferCompare(stringWrite,stringRead,strlen(stringWrite));
	 
	if(strFlag == 0)
	{
		//将stringRead中的数据通过串口打印
		printf("%s\n\r",stringRead);
		LED_G(ON);
		EEPROM_INFO("I2C(AT24C02)存储字符串测试成功\n\r");
	}
	else
	{
		LED_R(ON);
		EEPROM_INFO("I2C(AT24C02)存储字符串测试失败\n\r");
	}
}


uint8_t bufferCompare(uint8_t *buffer1, uint8_t *buffer2, uint8_t len)
{
	while(len--)
	{
		if(*buffer1 != *buffer2)
		{
			return 1;
		}
		buffer1++;
		buffer2++;
	}
	
	return 0;
}

