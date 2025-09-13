#ifndef BIGCARMOTORCONTROL_H
#define BIGCARMOTORCONTROL_H

#include "sys.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#define BIGCAR_RECEIVE_DATA_LEN			128

// �����¼���־λ
#define UART_RX_F1_EVENT_BIT (1 << 1) // ���ڽ���F1�¼���־
#define UART_RX_F2_EVENT_BIT (1 << 2) // ���ڽ���F2�¼���־

// ����С���ٶȷ���ṹ��
typedef struct
{
	uint8_t dir;   //����
	int16_t leftMotorSpeed;    //����PWM
	int16_t rightMotorSpeed;		//�ҵ��PWM
}bigCarSpeed;

// ����λ�������ٶ�������
typedef union
{
	int16_t speed;
	char speedByte[2];
}SpeedUnion;

// ����λ������rtk������
typedef union
{
	uint32_t rtk;
	uint8_t rtkByte[4];
}rtkUnion;

// ��λ�����͹�����GPS������
typedef union
{
	uint32_t gps;
	uint8_t gpsByte[4];
}gpsUnion;

// ��λ�����͹����Ļ�������������������
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

