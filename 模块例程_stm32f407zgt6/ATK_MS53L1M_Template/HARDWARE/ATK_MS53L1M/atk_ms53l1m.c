/**
 ****************************************************************************************************
 * @file        atk_ms53l1m.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-06-12
 * @brief       ATK-MS53L1M模块驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "atk_ms53l1m.h"
#include "delay.h"

/**
 * @brief       ATK-MS53L1M模块硬件初始化
 * @param       无
 * @retval      无
 */
void atk_ms53l1m_init(void)
{
	#if 0
  atk_ms53l1m_iic_init();
	#else
	Hardware_IIC_Init();
	#endif
  
	delay_ms(100);
}
