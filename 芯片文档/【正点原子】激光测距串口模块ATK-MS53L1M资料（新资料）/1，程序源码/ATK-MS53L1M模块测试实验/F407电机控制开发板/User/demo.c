/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS53L1Mģ�����ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� F407���������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "demo.h"
#include "./BSP/ATK_MS53L1M/atk_ms53l1m.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include <stdio.h>

/**
 * @brief       ��ʾ�豸��ַ
 * @param       ��
 * @retval      ��
 */
static void demo_show_id(uint16_t id)
{
    char buf[23];
    
    sprintf(buf, "ATK-MS53L1M ID: 0x%04x", id);
    
    printf("%s\r\n", buf);
    lcd_show_string(32, 151, 176, 16, 16, buf, BLUE);
}

/**
 * @brief       ����0���ܣ���ȡATK-MS53L1M����ֵ
 * @param       is_normal: 0��Modbusģʽ
 *                         1��Normalģʽ
 *              device_id: ATK-MS53L1M�豸��ַ
 * @retval      ��
 */
static void demo_key0_fun(uint8_t is_normal, uint16_t device_id)
{
    uint8_t ret;
    uint16_t dat;
    
    if (is_normal == 0)
    {
        /* ATK-MS53L1M Modbus����ģʽ��ȡ����ֵ */
        ret = atk_ms53l1m_modbus_get_data(device_id, &dat);
        if (ret == 0)
        {
            printf("[Modbus]Distance: %dmm\r\n", dat);
        }
        else
        {
            printf("Modbus mode get data failed!\r\n");
        }
    }
    else
    {
        /* ATK-MS53L1M Normal����ģʽ��ȡ����ֵ */
        ret = atk_ms53l1m_normal_get_data(&dat);
        if (ret == 0)
        {
            printf("[Normal]Distance: %dmm\r\n", dat);
        }
        else
        {
            printf("Normal mode get data failed!\r\n");
        }
    }
}

/**
 * @brief       ����1���ܣ��л�ATK-MS53L1M����ģʽ
 * @param       is_normal: 0��Modbusģʽ
 *                         1��Normalģʽ
 *              device_id: ATK-MS53L1M�豸��ַ
 * @retval      ��
 */
static void demo_key1_fun(uint8_t *is_normal, uint16_t device_id)
{
    uint8_t ret;
    
    if (*is_normal == 0)
    {
        /* ����ATK-MS53L1M�Ĺ���ģʽΪNormalģʽ */
        ret = atk_ms53l1m_write_data(device_id, ATK_MS53L1M_FUNCODE_WORKMODE, ATK_MS53L1M_WORKMODE_NORMAL);
        if (ret == 0)
        {
            /* ����ATK-MS53L1M�Ļش�����Ϊ5Hz */
            atk_ms53l1m_write_data(device_id, ATK_MS53L1M_FUNCODE_BACKRATE, ATK_MS53L1M_BACKRATE_5HZ);
            /* ����ATK-MS53L1M�Ĳ���ģʽΪ������ģʽ */
            atk_ms53l1m_write_data(device_id, ATK_MS53L1M_FUNCODE_MEAUMODE, ATK_MS53L1M_MEAUMODE_LONG);
            
            *is_normal = 1;
            printf("Set to Normal mode.\r\n");
        }
        else
        {
            printf("Set Normal failed!\r\n");
        }
    }
    else
    {
        /* ����ATK-MS53L1M�Ĺ���ģʽΪModbusģʽ */
        ret = atk_ms53l1m_write_data(device_id, ATK_MS53L1M_FUNCODE_WORKMODE, ATK_MS53L1M_WORKMODE_MODBUS);
        if (ret == 0)
        {
            /* ����ATK-MS53L1M�Ĳ���ģʽΪ���������ģʽ */
            atk_ms53l1m_write_data(device_id, ATK_MS53L1M_FUNCODE_MEAUMODE, ATK_MS53L1M_MEAUMODE_LONG);
            
            *is_normal = 0;
            printf("Set to Modbus mode.\r\n"); 
        }
        else
        {
            printf("Set Modbus mode failed!\r\n");
        }
    }
}

/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
void demo_run(void)
{
    uint8_t ret;
    uint8_t key;
    uint16_t id;
    uint8_t is_normal = 0;
    
    /* ��ʼ��ATK-MS53L1M */
    ret = atk_ms53l1m_init(115200, &id);
    if (ret != 0)
    {
        printf("ATK-MS53L1M init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    /* ATK-MS53L1M��ʼ���ɹ�����ʾ�豸��ַ */
    demo_show_id(id);
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                /* ��ȡATK-MS53L1M����ֵ */
                demo_key0_fun(is_normal, id);
                break;
            }
            case KEY1_PRES:
            {
                /* �л�ATK-MS53L1M����ģʽ */
                demo_key1_fun(&is_normal, id);
                break;
            }
            default:
            {
                break;
            }
        }
        
        delay_ms(10);
    }
}
