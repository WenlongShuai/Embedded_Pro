#include "hardware_iic.h"
#include "delay.h"


/**
  * @brief  I2C1 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C2_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

	/* 使能与 I2C 有关的时钟 */
	IIC2_I2C_APBxClock_FUN(IIC2_I2C_CLK, ENABLE );
	IIC2_I2C_GPIO_APBxClock_FUN(IIC2_I2C_SCL_GPIO_CLK | IIC2_I2C_SDA_GPIO_CLK, ENABLE );
	
  /*!< GPIO configuration */
	/* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);
	
  /* I2C_SCL、I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	       // 开漏输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IIC2_I2C_SCL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IIC2_I2C_SDA_PIN;
  GPIO_Init(IIC2_I2C_SDA_PORT, &GPIO_InitStructure);
}


/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
void I2C_Mode_Config(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
	
	/* 初始化GPIO */
	I2C2_GPIO_Config();

  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	 
	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	/* 通信速率 */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

	/* I2C 初始化 */
  I2C_Init(IIC2_I2Cx, &I2C_InitStructure);
	/* 使能 I2C */
  I2C_Cmd(IIC2_I2Cx, ENABLE);  
	/* 启用 I2C 确认特性 */
//	I2C_AcknowledgeConfig(IIC2_I2Cx, ENABLE);
}


