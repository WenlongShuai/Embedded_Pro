/**
 ****************************************************************************************************
 * @file        atk_ms53l1m.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-06-12
 * @brief       ATK-MS53L1Mģ����������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� MiniSTM32 V4������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/ATK_MS53L1M/atk_ms53l1m.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       ATK-MS53L1Mģ��Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
void atk_ms53l1m_init(void)
{
    atk_ms53l1m_iic_init();
    delay_ms(100);
}
