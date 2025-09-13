/* 
使用STM32F103开发板实现的USART串口收发数据。
在bsp_usart1_5.h头文件中分别对USART1、USART2、USART3、UART4、UART5串口进行功能配置。
如果具体需要使用某一个串口时，我们只需要把相应的串口宏定义的值改为1
注意事项：
1、在使用中断接收串口数据时，需要把NVIC_USART_Config(); USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);屏蔽打开，打开之后就不能使用printf、scanf函数。
2、在printf、scanf函数源码中，他们分别是对标准输出流操作、标准输入流操作，所以我们在对串口进行操作时，需要自己重定向printf函数的fputc和scanf函数的fgetc函数。
*/