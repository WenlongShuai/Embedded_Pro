/**
 ****************************************************************************************************
 * @file        atk_ms53l1m.h
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

#ifndef __ATK_MS53L1M_H
#define __ATK_MS53L1M_H

#include "sys.h"
#include "atk_ms53l1m_iic.h"
#include "vl53l1_api.h"

#define ATK_MS53L1M_NUM 1

/* ATK-MS53L0M模块上电默认IIC通讯地址 */
#define ATK_MS53L1M_IIC_ADDR    0x52

/* ATK-MS53L0模块ID */
#define ATK_MS53L1M_MODULE_ID   0xEACC

typedef struct
{
    uint32_t TimingBudget;  /*测量周期,执行一次量程测量所需的时间,单位:us,范围[20ms, 1000ms] */
    uint32_t InterPeriod; /*测量周期之间的间隔时间 单位:ms, 范围[1ms, 1000ms]*/   

    /*
      1、如果测量间隔时间短于定时预算,则一旦设备完成测距,则下一个测距立即开始.
      2、传感器测距时不应调用计时预算和测量间期,用户必须停止测距,更改这些参数并重新启动测距.
    */
    
}vl53l1_mode; 

extern VL53L1_Dev_t dev;    /* vl53l1x设备 */

/* 操作函数 */
void atk_ms53l1m_init(uint8_t dir);

#endif
