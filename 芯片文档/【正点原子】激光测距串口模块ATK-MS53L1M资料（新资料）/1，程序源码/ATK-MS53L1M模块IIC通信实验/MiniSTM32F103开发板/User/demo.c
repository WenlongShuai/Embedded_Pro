/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-06-12
 * @brief       ATK-MS53L1Mģ�����ʵ��
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

#include "demo.h"
#include "./BSP/ATK_MS53L1M/atk_ms53l1m.h"
#include "vl53l1_i2c_platform.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include <stdio.h>

VL53L1_Dev_t dev;

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
    lcd_show_string(10, 96, 176, 16, 16, buf, BLUE);
}

/**
 * @brief       ����豸
 * @param       dev     : �豸
 *              iic_addr: �豸IICͨѶ��ַ
 * @retval      0: �豸����
 *              1: �豸����
 */
static void demo_detect_device()
{
    uint16_t module_id = 0;
    
    /* ��ȡ�豸ģ��ID */
    VL53L1_RdWord(&dev, 0x010F, &module_id);
    if (module_id != ATK_MS53L1M_MODULE_ID)
    {
        printf("ATK-MS53L1M Detect Failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    demo_show_id(module_id);
}

/**
 * @brief       �����豸
 * @param       dev: �豸
 * @retval      0: ���óɹ�
 *              1: ����ʧ��
 */
static uint8_t demo_config_device()
{
    VL53L1_Error ret = VL53L1_ERROR_NONE;
    
    /* �豸������� */ 
    ret = VL53L1_WaitDeviceBooted(&dev);
    
    /* �豸һ���Գ�ʼ�� */
    ret = VL53L1_DataInit(&dev);
    
    /* �豸������ʼ�� */
    ret = VL53L1_StaticInit(&dev);
    
    /* �����豸����ģʽ */
    ret = VL53L1_SetDistanceMode(&dev, VL53L1_DISTANCEMODE_LONG);
    
    /* �趨��������ʱ�� */
    ret = VL53L1_SetMeasurementTimingBudgetMicroSeconds(&dev, 140 * 1000);
    
    /* PER��Χ[1ms, 1000ms] */
    ret = VL53L1_SetInterMeasurementPeriodMilliSeconds(&dev, 150);
    
    /* ֹͣ���� */
    ret = VL53L1_StopMeasurement(&dev);
    
    /* ��ʼ���� */
    ret = VL53L1_StartMeasurement(&dev);
    
    return ret;
}

/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
void demo_run(void)
{
    int status;
    uint8_t data = 0;
    uint8_t data_ready = 0;
    
    VL53L1_RangingMeasurementData_t vl53l1_data;
    
    atk_ms53l1m_init();                        /* ģ���ʼ�� */
    demo_detect_device();                      /* ����豸 */
    demo_config_device();                      /* �����豸 */
    
    printf("ATK-MS53L1M Config Succedded!\r\n");
    
    while (1)
    {
        status = VL53L1_GetMeasurementDataReady(&dev, &data_ready);
        
        if (data_ready)
        {
            status = VL53L1_GetRangingMeasurementData(&dev, &vl53l1_data);
            
            if (status == 0)
            {
                data = vl53l1_data.RangeMilliMeter;
                printf("Distance: %d mm\r\n", data);
            }
            
            VL53L1_ClearInterruptAndStartMeasurement(&dev);
        }
        
        delay_ms(300);
    }
}
