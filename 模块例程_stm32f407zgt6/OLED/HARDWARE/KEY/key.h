#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// 按键状态定义
typedef enum {
    KEY_STATE_IDLE,        // 空闲状态
    KEY_STATE_PRESSED,     // 按下状态
    KEY_STATE_SHORT_RELEASED, // 短按释放状态
    KEY_STATE_WAIT_DOUBLE,   // 等待双击状态
		KEY_STATE_DOUBLE_PRESSED, //处理双击操作中第二次按下状态
    KEY_STATE_LONG_PRESS,    // 长按状态
} KeyState_t;

// 按键事件定义
typedef enum {
    KEY_EVENT_NONE,        // 无事件
    KEY_EVENT_CLICK,       // 单击事件
    KEY_EVENT_DOUBLE_CLICK,// 双击事件
    KEY_EVENT_LONG_PRESS,  // 长按事件
} KeyEvent_t;

// 按键结构体
typedef struct {
    KeyState_t state;      // 当前状态
    KeyEvent_t event;      // 按键事件
    uint32_t pressTime;    // 按下时间
    uint32_t releaseTime;  // 释放时间
    uint8_t pressed;       // 按键当前是否被按下
} Key_t;

// 按键引脚定义  KEY0
#define KEY0_GPIO_PORT     GPIOE
#define KEY0_GPIO_PIN      GPIO_Pin_4
#define KEY0_GPIO_CLK      RCC_AHB1Periph_GPIOE

// 按键引脚定义  KEY1
#define KEY1_GPIO_PORT     GPIOE
#define KEY1_GPIO_PIN      GPIO_Pin_3
#define KEY1_GPIO_CLK      RCC_AHB1Periph_GPIOE

void KEY_Init(void);
void Key_Process_K0(void);


#endif /* __KEY_H */
