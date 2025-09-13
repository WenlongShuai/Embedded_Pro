/**
 ****************************************************************************************************
 * @file        delay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-02-25
 * @brief       使用SysTick的普通计数模式对延迟进行管理(支持ucosii)
 *              提供delay_init初始化函数， delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2022-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20230206
 * 第一次发布
 * V1.1 20230225
 * 修改SYS_SUPPORT_OS部分代码, 默认仅支持UCOSII 2.93.01版本, 其他OS请参考实现
 * 修改delay_init不再使用8分频,全部统一使用MCU时钟
 * 修改delay_us使用时钟摘取法延时, 兼容OS
 * 修改delay_ms直接使用delay_us延时实现.
 *
 ****************************************************************************************************
 */

#include "sys.h"
#include "delay.h"

/* 添加公共头文件 (FreeRTOS 需要用到) */
#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

static uint32_t g_fac_us = 0;       /* us延时倍乘数 */

// 标准寄存器版本：递增系统滴答计数器
void SysTick_IncrementCounter(void)
{
	// 获取当前计数器值
	uint32_t current_tick = SysTick->VAL;
	
	// 获取重载值（SysTick周期）
	uint32_t reload_value = SysTick->LOAD;
	
	// 计算下一个滴答值
	uint32_t next_tick = (current_tick > 0) ? (current_tick - 1) : reload_value;
	
	// 更新计数器值
	SysTick->VAL = next_tick;
}


/**
 * @brief     systick中断服务函数,使用OS时用到
 * @param     ticks: 延时的节拍数
 * @retval    无
 */
void SysTick_Handler(void)
{
	 SysTick_IncrementCounter();
	/* OS 开始跑了,才执行正常的调度处理 */
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
}

/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率, 即CPU频率(HCLK), 216Mhz
 * @retval    无
 */
void delay_init(uint16_t sysclk)
{
#if SYS_SUPPORT_OS                          /* 如果需要支持OS. */
    uint32_t reload;
#endif
    SysTick->CTRL |= (1 << 2);              /* SYSTICK使用内部时钟源,频率为HCLK*/
    g_fac_us = sysclk;                      /* 不论是否使用OS,g_fac_us都需要使用 */
    SysTick->CTRL |= 1 << 0;                /* 使能Systick */
    SysTick->LOAD = 0XFFFFFF;             /* 注意systick计数器24位，所以这里设置最大重装载值 */
#if SYS_SUPPORT_OS                          /* 如果需要支持OS. */
    reload = sysclk;                        /* 每秒钟的计数次数 单位为M */
		reload *= 1000000 / configTICK_RATE_HZ;	/* reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右*/
    SysTick->CTRL |= 1 << 1;                /* 开启SYSTICK中断 */
    SysTick->LOAD = reload;                 /* 每1/delay_ostickspersec秒中断一次 */
#endif
}


/**
 * @brief     延时nus
 * @note      无论是否使用OS, 都是用时钟摘取法来做us延时
 * @param     nus: 要延时的us数
 * @note      nus取值范围: 0 ~ (2^32 / fac_us) (fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        /* LOAD的值 */
    ticks = nus * g_fac_us;                 /* 需要的节拍数 */

    told = SysTick->VAL;                    /* 刚进入时的计数器值 */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) 
            {
                break;                      /* 时间超过/等于要延迟的时间,则退出 */
            }
        }
    }
}

/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数 (0< nms <= (2^32 / fac_us / 1000))(fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_ms(uint16_t nms)
{
	uint32_t i;
	for (i=0; i<nms; i++)
	{
		delay_us(1000);
	}
}






