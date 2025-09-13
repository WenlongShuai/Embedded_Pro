#include "led.h"
void LED_Init(){
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOH,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOH,&GPIO_InitStruct);
}