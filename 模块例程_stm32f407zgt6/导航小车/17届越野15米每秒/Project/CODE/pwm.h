////////////////////////////////////////////////////////////////////////////////
/// @file    TIM3_PWM_Output.h
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FIRMWARE FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////


// Define to prevent recursive inclusion
#ifndef __TIM3_PWM_Input_H
#define __TIM3_PWM_Input_H
// Files includes
#include "hal_conf.h"
#include  "stdio.h"

#define Remote_Max_1 2045
#define Remote_Mid_1 1545
#define Remote_Min_1 1043

#define Remote_Max_0 2038
#define Remote_Mid_0 1536
#define Remote_Min_0 1035

#define Servo_Max 1700
#define Servo_Mid 1320 //舵机中位，需自行调整确定
#define Servo_Min 900

#define MODE_UP 2020
#define MODE_MID 1518
#define MODE_DOWN 1016
void Remote_Ctrl_Init(void);
void Remote_Ctrl(void);
void SERVO_SET(int value);
extern u16 period;
extern u16 duty;
extern u8 CollectFlag;
extern u16 period_1;
extern u16 duty_1;
extern u8 CollectFlag_1;
/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////

