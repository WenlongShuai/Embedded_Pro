#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include <stdlib.h>

/* 
stm32使用分散文件(sct)来控制变量分配到哪个指定的空间。
这里我们将内部的64KSRAM分为两部分，
第一部分前20K模拟作为内部的SRAM，
第二部分后40K模拟作为外部的SRAM，
我们将使用 _EXTER_ 修饰的全局变量分配到外部SRAM，
没有用 _EXTER_ 修饰的变量分配到内部SRAM，
_EXTER_ 只能修饰全局变量。
*/

//通过 _EXTER_ 修饰的变量存储在名字为 EXTER 节区
//__attribute__ 关键字只能修饰全局变量，不能修饰局部变量
#define _EXTER_ 	__attribute__ ((section("EXTER")))

void Delay(__IO u32 nCount); 

//定义变量到 模拟外部SRAM中
uint32_t _EXTER_ testValue  =7 ;  //RW-data
//定义变量到 模拟外部SRAM中
//本来全局变量初始值为0的属于ZI-data，但是链接器把testValue2当成了RW-data，
//这时链接器的本身特性，它把像数组这种初始值为0的才优化为ZI-data数据
uint32_t testValue2  = 0;  


//定义数组到 模拟外部SRAM中
uint8_t testGrup[100]  ={0};  //ZI-data
//定义数组到 模拟外部SRAM中
uint8_t _EXTER_ testGrup2[100] ={1,2,3}; //RW-data


/*本实验中的sct配置，若使用外部存储器时，堆区工作可能不正常，
  使用malloc无法得到正常的地址，不推荐在实际工程应用*/
/*另一种我们推荐的配置请参考教程中的说明*/

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint32_t inerTestValue =10;  //ZI-data
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
    
  /* 初始化串口 */
  USART_GPIO_Init();
  
  printf("\r\nSCT文件应用——优先使用内部SRAM，并把堆区分配到指定的空间 实验\r\n");
  
	printf("\r\n使用“	uint32_t inerTestValue = 10; ”语句定义的局部变量：\r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
  printf("\r\n使用“uint32_t _EXTER_ testValue  = 7 ;”语句定义的全局变量：\r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue,testValue);
	
  printf("\r\n使用“uint32_t testValue2  = 0 ; ”语句定义的全局变量：\r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d\r\n",(uint32_t)&testValue2,testValue2);
	
	
	printf("\r\n使用“uint8_t testGrup[100]  = {0};”语句定义的全局数组：\r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d,%d,%d\r\n",(uint32_t)&testGrup,testGrup[0],testGrup[1],testGrup[2]);
	
  printf("\r\n使用“uint8_t _EXTER_ testGrup2[100] = {1,2,3};”语句定义的全局数组：\r\n");
	printf("结果：它的地址为：0x%x,变量值为：%d，%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
	
/*本实验中的sct配置，若使用外部存储器时，堆区工作可能不正常，
  使用malloc无法得到正常的地址，不推荐在实际工程应用*/
  /*另一种我们推荐的配置请参考教程中的说明*/
	
	uint32_t * pointer = (uint32_t*)malloc(sizeof(uint32_t)*3); 
	if(pointer != NULL)
	{
		*(pointer)=1;
		*(++pointer)=2;
		*(++pointer)=3;	
		
		printf("\r\n使用“	uint32_t *pointer = (uint32_t*)malloc(sizeof(uint32_t)*3); ”动态分配的变量\r\n");
		printf("\r\n定义后的操作为：\r\n*(pointer++)=1;\r\n*(pointer++)=2;\r\n*pointer=3;\r\n\r\n");
		printf("结果：操作后它的地址为：0x%x,查看变量值操作：\r\n",(uint32_t)pointer); 
		printf("*(pointer--)=%d, \r\n",*(pointer--));
		printf("*(pointer--)=%d, \r\n",*(pointer--));
		printf("*(pointer)=%d, \r\n",*(pointer));
		
		free(pointer);
	}
	else
	{
		printf("\r\n使用malloc动态分配变量出错！！！\r\n");	
	}
  
  LED_R(ON);
	

  while(1); 
 
	return 0;

}


void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

