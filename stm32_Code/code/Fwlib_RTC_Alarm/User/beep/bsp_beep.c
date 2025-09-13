#include "bsp_beep.h"


void BEEP_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	BEEP_GPIO_CLK_FUNC(BEEP_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = BEEP_GPIO_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStruct);
}


void BEEP_Cmd(FunctionalState NewState)
{
	/* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if(NewState != DISABLE)
	{
		GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	}
	else
	{
		GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	}
}
