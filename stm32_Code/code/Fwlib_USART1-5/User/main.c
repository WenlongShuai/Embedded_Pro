#include "stm32f10x.h"
#include "bsp_usart1_5.h"
#include "bsp_systick.h"

int main()
{
	char a = 0;
	uint8_t arry[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
	uint8_t *str = "使用STM32F103开发板\n";
	
	USART_GPIO_Init();
	
	//Usart_SendByteData(USARTx, 100);
	//Usart_SendHalfWordData(USARTx, 0xffaa);
	//Usart_SendArrayData(USARTx, arry, sizeof(arry)/sizeof(arry[0]));
	//Usart_SendStrData(USARTx, str);
	
	printf("串口printf调试\n");
	while(1)
	{
		scanf("%c",&a);
		//a = getchar();
		printf("%c", a);
		SysTick_FWLIB_Delay_us(500);
	}
	
	return 0;
}






