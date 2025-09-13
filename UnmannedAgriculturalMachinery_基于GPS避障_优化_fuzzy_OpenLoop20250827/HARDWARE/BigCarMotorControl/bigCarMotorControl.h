#ifndef BIGCARMOTORCONTROL_H
#define BIGCARMOTORCONTROL_H

#include "sys.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#define BIGCAR_RECEIVE_DATA_LEN			128

// 定义事件标志位
#define UART_RX_F1_EVENT_BIT (1 << 1) // 串口接收F1事件标志
#define UART_RX_F2_EVENT_BIT (1 << 2) // 串口接收F2事件标志

// 设置小车速度方向结构体
typedef struct
{
	uint8_t dir;   //方向
	int16_t leftMotorSpeed;    //左电机PWM
	int16_t rightMotorSpeed;		//右电机PWM
}bigCarSpeed;

// 给下位机发送速度联合体
typedef union
{
	int16_t speed;
	char speedByte[2];
}SpeedUnion;

// 给下位机发送rtk联合体
typedef union
{
	uint32_t rtk;
	uint8_t rtkByte[4];
}rtkUnion;

// 下位机发送过来的GPS联合体
typedef union
{
	uint32_t gps;
	uint8_t gpsByte[4];
}gpsUnion;

// 下位机发送过来的霍尔传感器数据联合体
typedef union
{
	uint32_t hallSensor;
	uint8_t hallSensorByte[4];
}hallSensorUnion;

void bigCarMotorControlInit(void);
void UART4_Send_BufferData(const uint8_t *buffer, uint16_t len);
bigCarSpeed bigCarMotor_Set_Speed(float speed0, float speed1);
void getBigCarMotorSpeed(void);

void bigCarMotorDataPack(uint8_t type, SpeedUnion *SpeedUnionParam, rtkUnion *rtkUnionParam, uint8_t *sendDataBuf);
void bigCarMotorDataUnpack(uint8_t type, hallSensorUnion *hallSensorUnionParam, gpsUnion *gpsUnionParam, uint8_t *DataBuf);
void sendDataToBigCar(void);

#endif /* BIGCARMOTORCONTROL_H */

