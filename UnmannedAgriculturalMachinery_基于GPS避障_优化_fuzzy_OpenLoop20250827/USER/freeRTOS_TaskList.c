#include "freeRTOS_TaskList.h"
#include <string.h>
#include <stdlib.h>

#include "w25qxx.h"
#include "hi600.h"
#include "usart.h"
#include "bluetooth.h"
#include "fuzzy_decision.h"
#include "jy901p.h"
#include "jy901p_usart.h"
#include "fun.h"

#if !GETDISMODE
#include "hc_sr04.h"
#else
#include "atk_ms53l1m.h"
#endif

#if !DISMODE
#include "oled.h"
#else
#include "lcd_ili9341.h"
#include "gui.h"
#endif

#if defined(PID_ASSISTANT_EN) 
#include "protocol.h"
#include "timing_fun.h"
#include "pid_usart.h"
#endif

#if !CARTYPE
#include "motor.h"
#else
#include "bigCarMotorControl.h"
#endif


/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"  // �ź���������ͺͺ���������

extern uint8_t Flag_Left, Flag_Right, Turn_Flag;

extern uint8_t Flag_Direction;   //���ܱ�־λ
uint8_t emergencyCut_offFlag = 1;    //����ͣ����־λ

extern float RC_Velocity;

uint8_t triggerObstacleAvoidanceDir = 0;    //�������ϵķ���
uint8_t collectTaskStartFlag = 0;   //�ɼ�����ʼ��־λ
volatile static uint8_t greaterThresholdCount = 0;   // �洢����������ֵ�Ĵ���


FUZZY_ST fst;

#if !CARTYPE
extern float motorSpeedLeft;
extern float motorSpeedRight;
#else
extern float bigCarMotorSpeedLeft;
extern float bigCarMotorSpeedRight;
extern rtkUnion rtkUnionArray[2];
extern uint8_t rtkUpdateFlag;
extern uint8_t bigCar_ReceiveDataBuf[BIGCAR_RECEIVE_DATA_LEN];  // ���մ󳵴�����������
#endif

extern float speed_core[SPEED_SUM];

extern nmea_msg movingAverage_gpsx;
double gps_array[2][5];  //����γ�Ȼ���ƽ������

extern POSI_UNION posi_now;

extern QueueHandle_t xAppSerialCmdQueue;
extern QueueHandle_t xGPSSerialCmdQueue;
extern QueueHandle_t xUltrasonicQueue;
extern SemaphoreHandle_t xCreateTaskSemaphore;
extern EventGroupHandle_t xEventGroup;
QueueHandle_t xObstacleAvoidanceQueue = NULL;
QueueHandle_t xCollectGPSDataQueue = NULL;
TimerHandle_t xPeriodicTimer = NULL;

static float Measure_Distance[5] = {0.0f};

#if !GETDISMODE
extern ultrasonic_sensor_t ultrasonicSensors[SENSORNUM];
static float ultrasonic_array[5][2] = {0.0f};
#else
static uint8_t demo_detect_device(uint8_t dir);
static uint8_t demo_config_device(uint8_t dir);
extern VL53L1_Dev_t dev;
#endif

extern int receiveFlag;

extern volatile uint32_t systemTimeMs;

extern volatile float fAcc[3], fGyro[3], fMag[3], fAngle[3];

extern volatile char s_cDataUpdate, s_cCmd;

float currentFWValue = 0.0f;  //��ǰ��λֵ
float adjustedFWValue = 0.0f; //�����ϰ����Ҫ�����ķ�λֵ

uint8_t carMode = 0;  // ����ģʽ  1��ѭ��  2������ 3���ع�·��   4���ɵ�

#if OBSTACLE_AVOIDANCE_MODE
// �򵥱����б��������ϰ���ǰһ�̵�״̬
static uint8_t dirLast = 0;
static uint8_t carModeLast = 0;
#endif

//����ת����������ʻʱ��
uint16_t CalculateRunTimeAfterTurn(float theta_deg, float obstacle_distance, float obstacle_width);

static uint16_t travelTime = 0;
/******************************************************************************************************/
/*FreeRTOS����*/
void start_task(void);        /* ������ */
void DISTask(void);											/* ������ */
void IMUTask(void);             				/* ������ */
void getDistanceTask(void);             /* ������ */
void receiveAppDataTask(void);             /* ������ */
void gpsDataParseTask(void);             /* ������ */
void obstacleAvoidanceTask(void);										/* ������ */
void readCSBDataEventTask(void);							/* ������ */
void sendDataToAppTask(void);   							/* ������ */
void sendPWMToBigCarTask(void); 									/* ������ */
void receiveKeyDataTask(void);						/* ������ */
void receiveHallSersonGPSDataTask(void);

void vUltrasonicTimeoutCallback(TimerHandle_t xTimer);    //��������೬ʱ�ص�����
void vPeriodicTimerCallback(TimerHandle_t xTimer);				//����ִ��obstacleAvoidanceTask����Ļص�����


/* �������� */
static const char *taskName[TASK_NUM] = {"start_task", "DISTask", "IMUTask", "obstacleAvoidanceTimerTask",
																				 "readCSBDataEventTask", "getDistanceTask", "receiveAppDataTask", "gpsDataParseTask",
																					"sendDataToAppTask", "sendPWMToBigCarTask", "receiveKeyDataTask", "receiveHallSersonGPSDataTask"};
/* ������ */
static taskPtr taskListFun[TASK_NUM] = {start_task, DISTask, IMUTask, obstacleAvoidanceTask, 
																				readCSBDataEventTask, getDistanceTask, receiveAppDataTask, gpsDataParseTask,
																				sendDataToAppTask, sendPWMToBigCarTask, receiveKeyDataTask, receiveHallSersonGPSDataTask};
/* �������ȼ� */
static uint8_t taskPrio[TASK_NUM] = {0, 5, 28, 30, 29, 25, 22, 15, 6, 27, 7, 26};
/* �����ջ��С ��λ����(4Byte) */
static uint16_t taskStkSize[TASK_NUM] = {256, 256, 256, 256, 256, 256, 256, 512, 256, 256, 128, 128};
/* ������ */
TaskHandle_t StartTask_Handler = NULL;  
TaskHandle_t DISTask_Handler = NULL;
TaskHandle_t IMUTask_Handler = NULL;
TaskHandle_t getDistanceTask_Handler = NULL;
TaskHandle_t readCSBDataEventTask_Handler = NULL;
TaskHandle_t receiveAppDataTask_Handler = NULL;
TaskHandle_t gpsDataParseTask_Handler = NULL;
TaskHandle_t obstacleAvoidanceTask_Handler = NULL;
TaskHandle_t sendDataToAppTask_Handler = NULL;
TaskHandle_t sendPWMToBigCarTask_Handler = NULL;
TaskHandle_t receiveKeyDataTask_Handler = NULL;
TaskHandle_t receiveHallSersonGPSDataTask_Handler = NULL;
static TaskHandle_t *taskHandle[TASK_NUM] = {&StartTask_Handler, &DISTask_Handler, &IMUTask_Handler, &obstacleAvoidanceTask_Handler, 
																						 &readCSBDataEventTask_Handler, &getDistanceTask_Handler, &receiveAppDataTask_Handler, &gpsDataParseTask_Handler,
																						 &sendDataToAppTask_Handler, &sendPWMToBigCarTask_Handler, &receiveKeyDataTask_Handler, &receiveHallSersonGPSDataTask_Handler};

																						 
#if defined(PID_ASSISTANT_EN) 
extern struct PID_ST pid0, pid1;
/* �������� */
static const char *PIDTaskName = "receivePIDDataTask";
/* ������ */
void receivePIDDataTask(void);
/* �������ȼ� */
static uint8_t PIDTaskPrio = 30;
/* �����ջ��С ��λ����(4Byte) */
static uint16_t PIDTaskStkSize = 256;
/* ������ */
TaskHandle_t receivePIDDataTask_Handler;
#endif
																						 
static void oledChangeDataDis(uint8_t choose,float leftActualSpeed, float rightActualSpeed, float leftDistance, 
											float midDistance, float rightDistance, float leftSideDistance, float rightSideDistance, 
											double targetLon, double targetLat, double localLon, double localLat,
											float FW, uint8_t action, uint8_t mode)
{
	uint8_t disBuff[30] = {0};
	switch(choose)
	{
		case 0:
		{
			#if !DISMODE
			sprintf((char *)disBuff, "%.2f  ",leftActualSpeed);
			OLED_ShowStr(66, 0, disBuff, 1);
			sprintf((char *)disBuff, "%.2f  ",rightActualSpeed);
			OLED_ShowStr(66, 1, disBuff, 1);
			#else
			sprintf((char *)disBuff, "%.2f  ",leftActualSpeed);
			Lcd_Clear_Area(66,0,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,0,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.2f  ",rightActualSpeed);
			Lcd_Clear_Area(66,8,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,8,BLUE,WHITE,disBuff);
			#endif
			break;
		}
		case 1:
		{
			#if !DISMODE
			sprintf((char *)disBuff, "%.2f   ",leftDistance);
			OLED_ShowStr(0, 2, disBuff, 1);
			sprintf((char *)disBuff, "%.2f   ",midDistance);
			OLED_ShowStr(42, 2, disBuff, 1);
			sprintf((char *)disBuff, "%.2f   ",rightDistance);
			OLED_ShowStr(84, 2, disBuff, 1);
			sprintf((char *)disBuff, "%.2f   ",leftSideDistance);
			OLED_ShowStr(0, 3, disBuff, 1);
			sprintf((char *)disBuff, "%.2f   ",rightSideDistance);
			OLED_ShowStr(42, 3, disBuff, 1);
			#else
			sprintf((char *)disBuff, "%.2f   ",leftDistance);
			Lcd_Clear_Area(66,16,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,16,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.2f   ",midDistance);
			Lcd_Clear_Area(66,24,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,24,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.2f   ",rightDistance);
			Lcd_Clear_Area(66,32,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,32,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.2f   ",leftSideDistance);
			Lcd_Clear_Area(66,40,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,40,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.2f   ",rightSideDistance);
			Lcd_Clear_Area(66,48,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(66,48,BLUE,WHITE,disBuff);
			#endif
			break;
		}
		case 2:
		{
			#if !DISMODE
			sprintf((char *)disBuff, "%.7f,%.6f", localLon, localLat);
			OLED_ShowStr(0, 4, disBuff, 1);
			sprintf((char *)disBuff, "%.7f,%.6f", targetLon, targetLat);
			OLED_ShowStr(0, 5, disBuff, 1);
			#else
			sprintf((char *)disBuff, "%.7f", movingAverage_gpsx.longitude);
			Lcd_Clear_Area(42,56,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(42,56,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.7f", movingAverage_gpsx.latitude);
			Lcd_Clear_Area(42,64,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(42,64,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.7f", posi_now.wgs84.longitude);
			Lcd_Clear_Area(42,72,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(42,72,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%.7f", posi_now.wgs84.latitude);
			Lcd_Clear_Area(42,80,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(42,80,BLUE,WHITE,disBuff);
			#endif
			break;
		}
		case 3:
		{
			#if !DISMODE
			sprintf((char *)disBuff, "%.2f  ",FW);
			OLED_ShowStr(18, 6, disBuff, 1);
			sprintf((char *)disBuff, "%d   %d  ",action, mode);
			OLED_ShowStr(90, 6, disBuff, 1);
			#else
			sprintf((char *)disBuff, "%.2f  ",FW);
			Lcd_Clear_Area(24,88,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(24,88,BLUE,WHITE,disBuff);
			sprintf((char *)disBuff, "%d  %d",Mode, trackAndObstacleAvoidanceFlag);
			Lcd_Clear_Area(36,96,X_MAX_PIXEL-66,8,WHITE);
			Gui_DrawFont_GBK6x8(36,96,BLUE,WHITE,disBuff);
			#endif
			break;
		}
	}
}

static void oledFixedDataDis(void)
{
	#if !DISMODE
	OLED_ShowStr(0, 0, (unsigned char *)"L(mm/s) as:", 1);
	OLED_ShowStr(0, 1, (unsigned char *)"R(m/s)  as:", 1);
	OLED_ShowStr(0, 6, (unsigned char *)"fw:       mode:", 1);
	#else
	Lcd_Clear(WHITE);
	Gui_DrawFont_GBK6x8(0,0,RED,WHITE,"L(mm/s)as:");
	Gui_DrawFont_GBK6x8(0,8,RED,WHITE,"R(m/s)as:");
	
	Gui_DrawFont_GBK6x8(0,16,RED,WHITE,"L_Dis(cm):");
	Gui_DrawFont_GBK6x8(0,24,RED,WHITE,"M_Dis(cm):");
	Gui_DrawFont_GBK6x8(0,32,RED,WHITE,"R_Dis(cm):");
	Gui_DrawFont_GBK6x8(0,40,RED,WHITE,"LS_Dis(cm):");
	Gui_DrawFont_GBK6x8(0,48,RED,WHITE,"RS_Dis(cm):");
	
	Gui_DrawFont_GBK6x8(0,56,RED,WHITE,"L lon:");
	Gui_DrawFont_GBK6x8(0,64,RED,WHITE,"L lat:");
	Gui_DrawFont_GBK6x8(0,72,RED,WHITE,"T lon:");
	Gui_DrawFont_GBK6x8(0,80,RED,WHITE,"T lat:");
	
	Gui_DrawFont_GBK6x8(0,88,RED,WHITE,"fw:");
	
	Gui_DrawFont_GBK6x8(0,96,RED,WHITE,"mode:");
	#endif
}

#if WORKMODE
#if !OBSTACLE_AVOIDANCE_MODE   /* ģ������ */
FUZZY_ST detectObstacle(float leftDistance, float midDistance, float rightDistance, float leftSideDistance, float rightSideDistance)
{
	#if 1
	FUZZY_ST fuzzy_Res;  //���ģ�����߽��
	//��ת��2  ��ת��4 ���ˣ�3
	if (midDistance > MID_THRESHOLDVALUE && leftDistance > LEFT_THRESHOLDVALUE && rightDistance > RIGHT_THRESHOLDVALUE)
	{
		//ȫ��ǰ��
//		printf("RC_Velocity --> %.2f\r\n",RC_Velocity);
//		printf("motorSpeedLeft --> %.2f, motorSpeedRight --> %.2f\r\n",motorSpeedLeft, motorSpeedRight);
	
		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); //С�� ָ���ٶ�ǰ��
		#else	
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight); 	//�� ָ���ٶ�ǰ��
		#endif
		
		greaterThresholdCount = 0;
		
		fuzzy_Res = (FUZZY_ST){
			.fuzzyFlag = 0,
			.speed0 = 0,
			.speed1 = 0,
			.turnDir = 0,
			.turnAngle = 0
		};
	}
	else
	{
		if(greaterThresholdCount < 10)   
		{
			greaterThresholdCount++;
			vTaskDelay(pdMS_TO_TICKS(100));                           /* �����ʱ100ms */

			fuzzy_Res = (FUZZY_ST){
			.fuzzyFlag = 0,
			.speed0 = 0,
			.speed1 = 0,
			.turnDir = 0,
			.turnAngle = 0
			};
		}
		else   // ����3�δ�����ֵ����Ϊ���ϰ���Ŵ�������
		{
			greaterThresholdCount = 0;
			
			// �ж����С����ĸ����򴥷��ı��ϣ������ƶ�ʱ��ʱ��Ҫ���ϰ����ʼ���롣
			if(leftDistance <= LEFT_THRESHOLDVALUE)
			{
				triggerObstacleAvoidanceDir = 1;
			}
			else if(midDistance <= MID_THRESHOLDVALUE)
			{
				triggerObstacleAvoidanceDir = 2;
			}
			else if(rightDistance <= RIGHT_THRESHOLDVALUE)
			{
				triggerObstacleAvoidanceDir = 3;
			}
				
			fuzzy_Res = fuzzy_decision(leftDistance/100.0f, midDistance/100.0f, rightDistance/100.0f); //ʹ��ģ���߼����о���
			fuzzy_Res.fuzzyFlag = 1;
	//		printf("fuzzy_Res.speed0 = %.2f, fuzzy_Res.speed1 = %.2f\r\n",fuzzy_Res.speed0, fuzzy_Res.speed1);
			
			#if !CARTYPE
			Motor_Set_Speed(fuzzy_Res.speed0, fuzzy_Res.speed1);   //��ģ���߼����ߵĽ���õ�С�������
			#else
			bigCarMotor_Set_Speed(fuzzy_Res.speed0, fuzzy_Res.speed1); 	//��ģ���߼����ߵĽ���õ��󳵵����
			#endif
			
			if(fuzzy_Res.turnDir == 2 && rightSideDistance <= RIGHTSIDE_THRESHOLDVALUE && leftSideDistance > LEFTSIDE_THRESHOLDVALUE)
			{
				fuzzy_Res.turnDir = 4;
				fuzzy_Res.turnAngle = -fuzzy_Res.turnAngle;
			}
			else if(fuzzy_Res.turnDir == 4 && rightSideDistance > RIGHTSIDE_THRESHOLDVALUE && leftSideDistance <= LEFTSIDE_THRESHOLDVALUE)
			{
				fuzzy_Res.turnDir = 2;
				fuzzy_Res.turnAngle = -fuzzy_Res.turnAngle;
			}
			else if(rightSideDistance <= RIGHTSIDE_THRESHOLDVALUE && leftSideDistance <= LEFTSIDE_THRESHOLDVALUE)
			{
				fuzzy_Res.turnDir = 3;
				fuzzy_Res.turnAngle = 0;
			}
					
			vTaskDelay(pdMS_TO_TICKS(TIME_FUZZY_DELAY)); //��ʱ����ģ���߼����ߵĽ������һ��ʱ��
		}		
	}
	
	return fuzzy_Res;
	#endif
	
	#if 0
	//��ת��2  ��ת��4 ���ˣ�3
	if (midDistance > MID_THRESHOLDVALUE && leftDistance > LEFT_THRESHOLDVALUE && rightDistance > RIGHT_THRESHOLDVALUE)
	{
		//ȫ��ǰ��
//		printf("RC_Velocity --> %.2f\r\n",RC_Velocity);
//		printf("motorSpeedLeft --> %.2f, motorSpeedRight --> %.2f\r\n",motorSpeedLeft, motorSpeedRight);

		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); 						//С�� ָ���ٶ�ǰ��
		#else
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight); 	//�� ָ���ٶ�ǰ��
		#endif
		
		return 0;
	}
	else
	{
		fst = fuzzy_decision(leftDistance/100.0f, midDistance/100.0f, rightDistance/100.0f); //ʹ��ģ���߼����о���
//		printf("fuzzy_Res.speed0 = %.2f, fuzzy_Res.speed1 = %.2f\r\n",fuzzy_Res.speed0, fuzzy_Res.speed1);
		
		if(Flag_Direction != 0)
		{
			#if !CARTYPE
			Motor_Set_Speed(fst.speed0, fst.speed1);   //��ģ���߼����ߵĽ���õ�С�������
			#else
			bigCarMotor_Set_Speed(fst.speed0, fst.speed1); 	//��ģ���߼����ߵĽ���õ��󳵵����
			#endif
		}

		if(fst.turnDir == 2 && rightSideDistance <= RIGHTSIDE_THRESHOLDVALUE && leftSideDistance > LEFTSIDE_THRESHOLDVALUE)
		{
			fst.turnDir = 4;
			fst.turnAngle = -fst.turnAngle;
		}
		else if(fst.turnDir == 4 && rightSideDistance > RIGHTSIDE_THRESHOLDVALUE && leftSideDistance <= LEFTSIDE_THRESHOLDVALUE)
		{
			fst.turnDir = 2;
			fst.turnAngle = -fst.turnAngle;
		}
		else if(rightSideDistance <= RIGHTSIDE_THRESHOLDVALUE && leftSideDistance <= LEFTSIDE_THRESHOLDVALUE)
		{
			fst.turnDir = 3;
			fst.turnAngle = 0;
		}
		
		vTaskDelay(pdMS_TO_TICKS(TIME_FUZZY_DELAY)); //��ʱ����ģ���߼����ߵĽ������һ��ʱ��
		
		return fst.turnDir;
	}
	#endif
}
#endif
#endif

#if OBSTACLE_AVOIDANCE_MODE  /* �򵥱��� */
static uint8_t detectObstacle(float leftDistance, float midDistance, float rightDistance, float leftSideDistance, float rightSideDistance)
{
	if (midDistance <= MID_THRESHOLDVALUE || leftDistance <= LEFT_THRESHOLDVALUE || rightDistance <= RIGHT_THRESHOLDVALUE) //�����ϰ�������ֹͣ
	{
		if(greaterThresholdCount < 3)   
		{
			greaterThresholdCount++;
			vTaskDelay(pdMS_TO_TICKS(100));                           /* �����ʱ100ms */
			return 1;
		}
		#if !CARTYPE
		Motor_Set_Speed(0, 0);   //��ģ���߼����ߵĽ���õ�С�������
		#else
		bigCarMotor_Set_Speed(0, 0);   //��ģ���߼����ߵĽ���õ��󳵵����
		#endif
		
		return 0;
	}
	else  //û���ϰ����������ٶ�ǰ��
	{
		greaterThresholdCount = 0;
		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight);   //��ģ���߼����ߵĽ���õ�С�������
		#else
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight);			//��ģ���߼����ߵĽ���õ��󳵵����
		#endif
		return 1;
	}
}
#endif

/******************************************************************************************************/
/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
 void freertos_setStartTask(void)
{	
//	xObstacleAvoidanceQueue = xQueueCreate(5, sizeof(uint8_t));
//	configASSERT(xObstacleAvoidanceQueue != NULL);
	
	if(xGPSSerialCmdQueue == NULL)
	{
		// ������Ϣ���У�����Ϊ5���洢SerialCmd_t���ͣ������ڽ���GPS����
		xGPSSerialCmdQueue = xQueueCreate(5, HIP_USART_REC_LEN * sizeof(uint8_t));
		configASSERT(xGPSSerialCmdQueue != NULL);
	}

	if(xCreateTaskSemaphore == NULL)
	{
		// �����ź���,���ڽ���key�����µ��ź���
		xCreateTaskSemaphore = xSemaphoreCreateBinary();
		configASSERT(xCreateTaskSemaphore != NULL);
	}
	
	if(xCollectGPSDataQueue == NULL)
	{
		xCollectGPSDataQueue = xQueueCreate(5, 2*sizeof(double));
		configASSERT(xCollectGPSDataQueue != NULL);
	}

	if(xAppSerialCmdQueue == NULL)
	{
		// ������Ϣ���У�����Ϊ5���洢SerialCmd_t���ͣ������ڽ���APP����
		xAppSerialCmdQueue = xQueueCreate(5, 20*sizeof(uint8_t));
		configASSERT(xAppSerialCmdQueue != NULL);
	}
	
	if(xEventGroup == NULL)
	{
		// �����¼���־��,������󳵷������ݻ��߽�������
		xEventGroup = xEventGroupCreate();
		configASSERT(xEventGroup != NULL);
	}
	
	#if !GETDISMODE
	for(int i = 0;i < SENSORNUM;i++)	
	{
		ultrasonicSensors[i].sersor_ID = i+1;
		ultrasonicSensors[i].flag = 0;
		
		// �����¼���
    ultrasonicSensors[i].event_group = xEventGroupCreate();
		
		//��������
		ultrasonicSensors[i].data_queue = xQueueCreate(10, sizeof(ultrasonic_sensor_t));
		
		// ������ʱ��ʱ������������ID������
		ultrasonicSensors[i].timeout_timer = xTimerCreate(
				"US_TIM", 
				pdMS_TO_TICKS(50),
				pdFALSE, 
				(void*)&ultrasonicSensors[i], // ���ݴ�����ָ����ΪID
				vUltrasonicTimeoutCallback
		);
	}
	#endif
	
	/* ��ʼ��ģ�� */
	fuzzy_Init();
	Bluetooth_USART_GPIO_Init(115200);
	WitModuleInit();   //��ʼ��IMU 
	hi600_Init();  //hi600D��ʼ��
	oledFixedDataDis();
	bigCarMotorControlInit();   // ��ʼ����
	
	xTaskCreate((TaskFunction_t )taskListFun[0],            /* ������ */
							(const char*    )taskName[0],          /* �������� */
							(uint16_t       )taskStkSize[0],        /* �����ջ��С */
							(void*          )NULL,                  /* ������������Ĳ��� */
							(UBaseType_t    )taskPrio[0],       /* �������ȼ� */
							(TaskHandle_t*  )taskHandle[0]);   /* ������ */
	vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void)
{
	BaseType_t xReturned;
	
	taskENTER_CRITICAL();           /* �����ٽ��� */
	
	#if WORKMODE
	//���������ʱ��������ִ��obstacleAvoidanceTask����
	TimerHandle_t xPeriodicTimer = xTimerCreate(
			"PeriodicTimer",                // ��ʱ������
			pdMS_TO_TICKS(20), // ���ڣ�ת��Ϊʱ�ӽ��ģ�
			pdTRUE,                         // �Զ���װ��
			NULL,                           // �޲���
			vPeriodicTimerCallback          // �ص�����
	);
	configASSERT(xPeriodicTimer != NULL);
	xTimerStart(xPeriodicTimer, 0);    // ������ʱ��
	#endif
	
	for(int i = 1;i < TASK_NUM;i++)
	{
		if(i == 3)   
			continue;   //obstacleAvoidanceTask
		
		#if GETDISMODE
		if(i == 4)
			continue; //��������ȡ��������
		#endif
		
		#if CARTYPE
		if(i == 9 && GET_BLUETOOTH_STATE_LEVEL() == 0)  //��������δ����״̬
			continue; 	//����С���󳵶��������������ݵ������������
		#endif
		
		#if WORKMODE   // ����ģʽ
		if(i == 10)
			continue;
		#else      // �ɼ�ģʽ
		if(i == 2 || i == 3 || i == 4 || i == 5)
			continue;
		#endif	
		
		/* �������� */
		xReturned = xTaskCreate((TaskFunction_t )taskListFun[i],
							(const char*    )taskName[i],
							(uint16_t       )taskStkSize[i],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[i],
							(TaskHandle_t*  )taskHandle[i]);
									
							
		if(xReturned == pdPASS)
		{
			printf("%d create success\r\n",i);
		}
		else
		{
			printf("%d create fail\r\n",i);
		}
	}
	
	#if 0
	//��ʾ
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[1],
//							(const char*    )taskName[1],
//							(uint16_t       )taskStkSize[1],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[1],
//							(TaskHandle_t*  )taskHandle[1]);
							
	//IMU
	xReturned = xTaskCreate((TaskFunction_t )taskListFun[2],
							(const char*    )taskName[2],
							(uint16_t       )taskStkSize[2],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[2],
							(TaskHandle_t*  )taskHandle[2]);
							
	//����
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[3],
//							(const char*    )taskName[3],
//							(uint16_t       )taskStkSize[3],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[3],
//							(TaskHandle_t*  )taskHandle[3]);
							
	//��ȡ����������
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[4],
//							(const char*    )taskName[4],
//							(uint16_t       )taskStkSize[4],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[4],
//							(TaskHandle_t*  )taskHandle[4]);
							
	//����������
	xReturned = xTaskCreate((TaskFunction_t )taskListFun[5],
							(const char*    )taskName[5],
							(uint16_t       )taskStkSize[5],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[5],
							(TaskHandle_t*  )taskHandle[5]);
							
	//����APP����
	xReturned = xTaskCreate((TaskFunction_t )taskListFun[6],
							(const char*    )taskName[6],
							(uint16_t       )taskStkSize[6],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[6],
							(TaskHandle_t*  )taskHandle[6]);
							
	//GPS��ȡ����
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[7],
//							(const char*    )taskName[7],
//							(uint16_t       )taskStkSize[7],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[7],
//							(TaskHandle_t*  )taskHandle[7]);
							

	#endif

	vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
	taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}


void DISTask(void)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(150);  // 150ms����
	uint8_t count = 0;
	
	while(1)
	{
		float yawTemp = 0.0f;
		
		yawTemp = angle_format(-fAngle[2]);
		oledChangeDataDis(count%4, RC_Velocity, RC_Velocity, Measure_Distance[0], 
											Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4], 
											posi_now.wgs84.longitude, posi_now.wgs84.latitude, movingAverage_gpsx.longitude, movingAverage_gpsx.latitude, yawTemp, Flag_Direction, carMode);
//		oledChangeDataDis(3, 0, 0, 0, 0, 0, 0, 0 , 0, Flag_Direction);
		count++;
		
//		vTaskDelay(pdMS_TO_TICKS(200));                           /* �����ʱ200ms */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* ������ʱ200ms */
	}
}

void IMUTask(void)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10);  // 10ms����
		
	while(1)
	{
		#if defined(IICMODE)
		WitReadReg(AX, 12);
		#endif
		
		if(s_cDataUpdate)
		{
			for(int i = 0; i < 3; i++)
			{
			fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f;
			fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
			fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
			fMag[i] = sReg[HX+i]*1.0;
			}
			if(s_cDataUpdate & ACC_UPDATE)
			{
				#if defined(USARTMODE)
				printf("acc:%.3f %.3f %.3f\r\n", fAcc[0], fAcc[1], fAcc[2]);
				#endif
				s_cDataUpdate &= ~ACC_UPDATE;
			}
			if(s_cDataUpdate & GYRO_UPDATE)
			{
				#if defined(USARTMODE)

				printf("gyro:%.3f %.3f %.3f\r\n", fGyro[0], fGyro[1], fGyro[2]);
				#endif
				s_cDataUpdate &= ~GYRO_UPDATE;
			}
			if(s_cDataUpdate & ANGLE_UPDATE)
			{
				#if defined(USARTMODE)
				printf("angle:%.3f %.3f %.3f\r\n", fAngle[0], fAngle[1], angle_format(-fAngle[2]));
				#endif
				s_cDataUpdate &= ~ANGLE_UPDATE;
			}
			if(s_cDataUpdate & MAG_UPDATE)
			{
				#if defined(USARTMODE)
				printf("mag:%.3f %.3f %.3f\r\n", fMag[0], fMag[1], fMag[2]);
				#endif
				s_cDataUpdate &= ~MAG_UPDATE;
			}
			
			currentFWValue = angle_format(-fAngle[2]);
		}
    vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* ������ʱ10ms */
	}
}

void getDistanceTask(void)
{
	#if !GETDISMODE
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(12*SENSORNUM);  // 12*SENSORNUM ms����
	Hcsr04Init();  //��������ʼ��
	#else
	int status;
	float sum = 0.0f;
	uint8_t num = 0;
	uint8_t data_ready = 0;
	float laser_array[5][2] = {0.0f};
	
	VL53L1_RangingMeasurementData_t vl53l1_data;
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);  // 200ms����
	
	for(int i = 1;i <= ATK_MS53L1M_NUM;i++)
	{
		atk_ms53l1m_init(i);                        /* ģ���ʼ�� */
		demo_detect_device(i);                      /* ����豸 */
		demo_config_device(i);                      /* �����豸 */
//		printf("ATK-MS53L1M [%d] Config Succedded!\r\n",i);
	}

	#endif
		
	while(1)
	{
		#if !GETDISMODE
		for(int i = 0;i < SENSORNUM;i++)
		{				
			// �����ô������ĳ�ʱ��ʱ��
			xTimerStart(ultrasonicSensors[i].timeout_timer, pdMS_TO_TICKS(5));
			
			Hcsr04Start(i);	//����������ģ����
			
			vTaskDelay(pdMS_TO_TICKS(12)); // ���ȴ�12ms����ӦԼ2�ף�
		}
    vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* ������ʱ xFrequency ms */
		#else
				
		for(int i = 0;i < ATK_MS53L1M_NUM;i++)
		{
			status = VL53L1_GetMeasurementDataReady(i+1, &dev, &data_ready);
		
			if (data_ready)
			{
					status = VL53L1_GetRangingMeasurementData(i+1, &dev, &vl53l1_data);
					
					if (status == 0)
					{
						laser_array[i][num%2] = (float)(vl53l1_data.RangeMilliMeter/10.0f);
//						Measure_Distance[i] = (float)(vl53l1_data.RangeMilliMeter/10.0f);
//						printf("[%d] Distance: %.2f cm\r\n",i, (float)(vl53l1_data.RangeMilliMeter/10.0f));
//						printf("laser_array[%d][%d]: %.2f cm\r\n",i,num%2, laser_array[i][num%2]);
					}			
//					VL53L1_ClearInterruptAndStartMeasurement(i+1, &dev);
			}
			else
			{
				demo_detect_device(i+1);                      /* ����豸 */
				demo_config_device(i+1);                      /* �����豸 */
			}
		}
		num++;
		
		for(int i = 0;i < ATK_MS53L1M_NUM;i++)
		{
			sum = 0;
			for(int j = 0;j < 2;j++)
			{
				sum += laser_array[i][j];
			}
			Measure_Distance[i] = sum / 2.0f;
			
//			printf("Measure_Distance[%d]=%f\r\n",i,Measure_Distance[i]);
		}
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* ������ʱ xFrequency ms */
		#endif
	}
}

//����ʱ��ʱ������ʱ��˵��û���յ��ز�����ʱ���ͳ�ʱ��Ϣ������
void vUltrasonicTimeoutCallback(TimerHandle_t xTimer) 
{
	#if !GETDISMODE
   // �Ӷ�ʱ��ID��ȡ������ָ��
   ultrasonic_sensor_t* sensor = (ultrasonic_sensor_t*)pvTimerGetTimerID(xTimer);
	
		// ��ǳ�ʱ״̬
    sensor->flag = 0;
    
    // ���ͳ�ʱ�¼�
    xEventGroupSetBits(sensor->event_group, ULTRASONIC_TIMEOUT_BIT);
    
    // ������Ч����
    ultrasonic_data_t data = {
        .distance_cm = 0,
        .is_valid = 0,
        .sensor_id = sensor->sersor_ID
    };
		
		xQueueSend(sensor->data_queue, &data, 0);
	#endif
}


void readCSBDataEventTask(void)
{
	#if !GETDISMODE
	EventBits_t uxBits;
	ultrasonic_data_t data;
	
	uint8_t n = 0;
	
	while(1)
	{
		// �������ÿ�����������¼�
		for (int i = 0; i < SENSORNUM; i++) {
				ultrasonic_sensor_t* sensor = &ultrasonicSensors[i];
				
				// ����Ƿ����¼�����������
				uxBits = xEventGroupGetBits(sensor->event_group);
				
				if (uxBits & (ULTRASONIC_DATA_READY_BIT | ULTRASONIC_TIMEOUT_BIT)) {
						// ����¼�λ
						xEventGroupClearBits(sensor->event_group, 
								ULTRASONIC_DATA_READY_BIT | ULTRASONIC_TIMEOUT_BIT);
						
						// �Ӷ��л�ȡ����
						if (xQueueReceive(sensor->data_queue, &data, 0) == pdPASS) {
								if (data.distance_cm != 0) {
										// ������Ч����
										printf("Sensor %d: %.2f cm\n", data.sensor_id, data.distance_cm);
									
										// �����������ݸ��µ�����������
//										ultrasonic_Distance[data.sensor_id-1] = data.distance_cm;
									ultrasonic_array[data.sensor_id-1][(n++)%2] = data.distance_cm;
												
//										xQueueSend(xObstacleAvoidanceQueue, &ret, portMAX_DELAY);
								} 
								else {
										// ����ʱ
										printf("Sensor %d timeout!\n", data.sensor_id);
//										ultrasonic_Distance[data.sensor_id-1] = 400;
									ultrasonic_array[data.sensor_id-1][(n++)%2] = 200;
									// ���ô�����������׼���´β���
											ultrasonicSensors[data.sensor_id-1].flag = 0;
											ultrasonicSensors[data.sensor_id-1].start_time = 0;
											ultrasonicSensors[data.sensor_id-1].overflow_cnt = 0;
											ultrasonicSensors[data.sensor_id-1].time_diff = 0;
											ultrasonicSensors[data.sensor_id-1].end_time = 0;
									
										switch(sensor->sersor_ID)
										{
											case 1:
												TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//����Ϊ�����ز���
												break;
											case 2:
												TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//����Ϊ�����ز���
												break;
											case 3:
												TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//����Ϊ�����ز���
												break;
											case 4:
												TIM_OC1PolarityConfig(TIM9, TIM_ICPolarity_Rising);		//����Ϊ�����ز���
												break;
											case 5:
												TIM_OC2PolarityConfig(TIM9, TIM_ICPolarity_Rising);		//����Ϊ�����ز���
												break;
										}   
									}
								}
						
								float sum = 0.0f;
								for(int j = 0;j < 2;j++)
								{
									sum += ultrasonic_array[data.sensor_id-1][j];
								}
								Measure_Distance[data.sensor_id-1] = sum / 2;
						}
				}
			// ���������Ա���ռ�ù���CPU
			vTaskDelay(pdMS_TO_TICKS(20));
		}
		#endif
}


void receiveAppDataTask(void)
{
	u8 Control_Command_BUF[20] = {0};
	
	while(1)
	{
		// �����ȴ�������Ϣ�����õȴ���
		if (xQueueReceive(xAppSerialCmdQueue, Control_Command_BUF, portMAX_DELAY) == pdTRUE) 
		{
			char *disconnectStr = strstr((char *)Control_Command_BUF, (char *)"+DISC:SUCCESS");
			char *connectStr = strstr((char *)Control_Command_BUF, (char *)"+CONNECTED");

			if((disconnectStr && strlen(disconnectStr) > 12))   //����Ͽ�����
			{
				Flag_Direction = 0;
				emergencyCut_offFlag = 1;
				memset(Control_Command_BUF, 0, 20);
				
				#if CARTYPE
				/* ɾ������ */
				taskENTER_CRITICAL();           /* �����ٽ��� */
				if(*taskHandle[9] != NULL)
				{
					vTaskDelete(*taskHandle[9]);  /* ɾ�����͵��󳵵��������� */
					*taskHandle[9] = NULL;
				}
				taskEXIT_CRITICAL();            /* �˳��ٽ��� */
				#endif
				continue;
			}
			else if((connectStr && strlen(connectStr) > 9) )    //����������
			{
				Flag_Direction = 0;
				memset(Control_Command_BUF, 0, 20);
				
				#if CARTYPE
				taskENTER_CRITICAL();           /* �����ٽ��� */

				if(*taskHandle[9] == NULL)
				{
					/* �������� */
					xTaskCreate((TaskFunction_t )taskListFun[9],
										(const char*    )taskName[9],
										(uint16_t       )taskStkSize[9],
										(void*          )NULL,
										(UBaseType_t    )taskPrio[9],
										(TaskHandle_t*  )taskHandle[9]);
				}		
				taskEXIT_CRITICAL();            /* �˳��ٽ��� */
				#endif
				continue;
			}
			else if(Control_Command_BUF[0] >= 0x40 && Control_Command_BUF[0] <= 0x5F)    // �����˶�����
			{
				Flag_Direction = Control_Command_BUF[0] - 0x40;
				if(Flag_Direction == 5)
				{
					RC_Velocity -= 100.0f;  //���ٰ�����-100mm/s
				}
				else if(Flag_Direction == 6)
				{
					RC_Velocity += 100.0f; //���ٰ�����+100mm/s
				}

				#if !CARTYPE
				motorSpeedLeft = motorSpeedRight = RC_Velocity/100.0f;
				#else
				bigCarMotorSpeedLeft = bigCarMotorSpeedRight = RC_Velocity/100.0f;
				#endif
				
				speed_core[0] = (RC_Velocity/100)<=0 ? 1 : RC_Velocity/100.0f;
				speed_core[1] = (RC_Velocity/100 - 1)<=0 ? 1 : RC_Velocity/100.0f - 1;
				
				#if defined(PID_ASSISTANT_EN)
				Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); //ȫ��ǰ��
				#endif
				
				if(RC_Velocity < 0)
					RC_Velocity = 0; 	
				
				if(Flag_Direction == 0)
				{
					emergencyCut_offFlag = 1;
					#if !CARTYPE
					TIM_SetCompare1(MOTOR_PWM_TIMx, 0);
					TIM_SetCompare2(MOTOR_PWM_TIMx, 0);
					#else
					bigCarMotorSpeedLeft = 0;
					bigCarMotorSpeedRight = 0;
					#endif
				}
				else
				{
					if(Flag_Direction == 5 || Flag_Direction == 6)  // ������µ��Ǽ��١����ٰ�������ֱ���˳�
						continue;
					
					emergencyCut_offFlag = 0;
					#if WORKMODE   // ����ģʽ
					carMode = 1;
					#endif
				}
				
				#if OBSTACLE_AVOIDANCE_MODE
				dirLast = Flag_Direction;
				carModeLast = carMode;
				#endif
			}
			memset(Control_Command_BUF, 0, 20);
		}
	}
}





void gpsDataParseTask(void)
{
	uint8_t HIP_USART_RX_BUF[HIP_USART_REC_LEN] = {0};

	while(1)
	{
		// �����ȴ�������Ϣ�����õȴ���
		if (xQueueReceive(xGPSSerialCmdQueue, HIP_USART_RX_BUF, 1000) == pdTRUE)
		{
			#if 0
			char latBuff[12] = {0};
			char lonBuff[12] = {0};
			char *latStr = strstr((char *)HIP_USART_RX_BUF, "lat:");
			char *lonStr = strstr((char *)HIP_USART_RX_BUF, "lon:");
			
			memcpy(latBuff, latStr+4, 10);
			memcpy(lonBuff, lonStr+4, 11);
			
			char *endptr;  // ����ָ��δ��ת���Ĳ���
			
		//	printf("HIP_USART_RX_BUF --> %s\r\n",HIP_USART_RX_BUF);
//		printf("latBuff --> %s\r\n",latBuff);
//		printf("lonBuff --> %s\r\n",lonBuff);
			movingAverage_gpsx.longitude = strtod(lonBuff, &endptr);
			movingAverage_gpsx.latitude = strtod(latBuff, &endptr);
//			printf("%s\r\n",HIP_USART_RX_BUF);
//			printf("longitude ---> %.7f,latitude --> %.7f\r\n",movingAverage_gpsx.longitude,movingAverage_gpsx.latitude);
			
			#endif
			
			nmea_msg temp;
			GPS_Analysis(&temp, HIP_USART_RX_BUF);
			movingAverage_gpsx.longitude = temp.longitude;
			movingAverage_gpsx.latitude = temp.latitude;

			
			#if CARTYPE
			rtkUpdateFlag = 1;
			uint32_t sendLonTemp = movingAverage_gpsx.longitude*10000000;
			uint32_t sendLatTemp = movingAverage_gpsx.latitude*10000000;
			memcpy(rtkUnionArray, &sendLonTemp, 4);
			memcpy(rtkUnionArray+1, &sendLatTemp, 4);
			#endif
			
			#if !WORKMODE   // �ɼ�ģʽ
			
			#if COLLECT_GPS_MODE  // �ֶ��ɼ�
			if(Flag_Direction == 17)   //APP�Ϸ��Ͳɼ�GPS���ݰ�ť
			{
				Flag_Direction = 0;
			#else   //�Զ��ɼ�
			if(collectTaskStartFlag == 1)
			{
			#endif
				double GPSDataBuf[2] = {0};
				GPSDataBuf[0] = movingAverage_gpsx.longitude;
				GPSDataBuf[1] = movingAverage_gpsx.latitude;
				xQueueSend(xCollectGPSDataQueue, GPSDataBuf, 0);
			}	
			#endif
		}
		
		#if WORKMODE  // ����ģʽ
		if(carMode == 1)
			Track();//ѭ������
		
		#endif
	}
}

void vPeriodicTimerCallback(TimerHandle_t xTimer)
{
	if(Measure_Distance[1] != 0 && emergencyCut_offFlag != 1)   //��ֹ����ʱ��������Ϊ0���������ϡ�emergencyCut_offFlag����ֹͣ
	{
//		obstacleAvoidanceTask();
	}
}
	
	
void obstacleAvoidanceTask(void)
{
	#if WORKMODE
	#if !OBSTACLE_AVOIDANCE_MODE   /* ģ���㷨���� */
	#if 1
	static uint8_t turnDirArray[20] = {0};
	static float turnAngleArray[20] = {0.0f};
	static int8_t count = 0;    //��¼�����ϰ���Ĵ���
	static uint8_t avoidanceCompleteFlag = 0;   //������ɱ�־   
	static uint8_t returnCompleteFlag = 0;    //�ع�ģʽ�·���·����ɱ�־

//printf("--->%f,%f,%f,%f,%f\r\n",ultrasonic_Receive[0].distance, ultrasonic_Receive[1].distance, ultrasonic_Receive[2].distance, ultrasonic_Receive[3].distance, ultrasonic_Receive[4].distance);
	
	FUZZY_ST fuzzy_Temp;    //���ģ�����ߵĽ��
	//���ݲ��������ȡģ�����ߵĽ��������ת��Ƕȼ�����
	fuzzy_Temp = detectObstacle(Measure_Distance[0], Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4]);
		
	if(fuzzy_Temp.fuzzyFlag)    //ǰ�����ϰ���
	{
//		printf("1 adjustedFWValue = %.2f, Flag_Direction = %d\r\n",fuzzy_Temp.turnAngle, fuzzy_Temp.turnDir);
		turnDirArray[count] = fuzzy_Temp.turnDir;   //��¼ת��ǰ�ķ���
		turnAngleArray[count] = currentFWValue; //��¼ת��ǰ�ĽǶ�
		adjustedFWValue = fuzzy_Temp.turnAngle + currentFWValue; //С����Ҫת�����ĽǶ�
		Flag_Direction = fuzzy_Temp.turnDir;    //С��ת������
		count++;
		
		carMode = 2;   //�򿪱���ģʽ
		
		switch(triggerObstacleAvoidanceDir)
		{
			case 1:
				travelTime = CalculateRunTimeAfterTurn(fabs(fuzzy_Temp.turnAngle), Measure_Distance[0]/100, 0)/2;
				break;
			case 2:
				travelTime = CalculateRunTimeAfterTurn(fabs(fuzzy_Temp.turnAngle), Measure_Distance[1]/100, 0)/2;
				break;
			case 3:
				travelTime = CalculateRunTimeAfterTurn(fabs(fuzzy_Temp.turnAngle), Measure_Distance[2]/100, 0)/2;
				break;
		}
		
//		printf("fuzzy_Temp.turnAngle = %.2f\r\n",fuzzy_Temp.turnAngle);
//				printf("Measure_Distance[0] = %.2f\r\n",Measure_Distance[0]);
		
		uint16_t timeCount = 0;
		do
		{
			if(fabs(currentFWValue - adjustedFWValue) <= 5.0)   //ת��ָ���Ƕ�
			{
//				printf("1 yes\r\n");
				adjustedFWValue = 370;
				Flag_Direction = 0;  //ֹͣ
				vTaskDelay(pdMS_TO_TICKS(2000));   //�ȴ�2��
				break;
			}
			else   // δת��ָ���Ƕ�
			{
				//�ȴ�10s��ʱ
				vTaskDelay(pdMS_TO_TICKS(10));   //�ȴ�10ms
				timeCount++;
				if(timeCount == 1000)   //��ʱ
				{
					WitModuleInit();   //���³�ʼ��IMU
				}
				else if(timeCount > 1500)   //����IMUģ��֮��δת��ָ���Ƕ�
				{
					Flag_Direction = 3;  //����
					vTaskDelay(pdMS_TO_TICKS(3000));   //�ȴ�3��
					carMode = 1;  //��ѭ��ģʽ
					Flag_Direction = 1;   //ǰ��
				}
			}
		}
		while(timeCount < 1500 && emergencyCut_offFlag != 1);
	}
	else if(!fuzzy_Temp.fuzzyFlag && carMode != 1)   //ǰ��û���ϰ���
	{
		if(carMode == 2 && avoidanceCompleteFlag != 1)  //����ģʽ
		{
			Flag_Direction = 1;   //ǰ��
			avoidanceCompleteFlag = 1;
			vTaskDelay(travelTime);   //�ȴ�travelTime����
		}
		else if(carMode == 2 && avoidanceCompleteFlag == 1)
		{
			avoidanceCompleteFlag = 0;
			carMode = 3;
		}
		
		if(carMode == 3)   //�ع�·��ģʽ
		{
//			printf("count=%d\r\n",count);
			
			if(returnCompleteFlag)
			{
				returnCompleteFlag = 0;
				Flag_Direction = 1;
				vTaskDelay(travelTime);   //�ȴ�travelTime����
			}
			
			if(--count >= 0)   //ת������ǰ��λ��
			{
//				printf("turnDirArray[%d]=%d,turnAngleArray[%d]=%.2f\r\n",count,turnDirArray[count],count,turnAngleArray[count]);
				Flag_Direction = turnDirArray[count] == 2 ? 4 : 2;
				adjustedFWValue = turnAngleArray[count];
			}
			else
			{
				returnCompleteFlag = 0;
				avoidanceCompleteFlag = 0;
				count = 0;
				Flag_Direction = 1;
				carMode = 1; //������ɣ�����ѭ��ģʽ��Flag_Direction����ֵ��trackѭ������
				memset(turnAngleArray, 0, 20);
				memset(turnDirArray, 0, 20);
				return;
			}
			
			uint16_t timeCount2 = 0;
			
			do
			{
				if(fabs(currentFWValue - adjustedFWValue) <= 5.0)   //ת��ָ���Ƕ�
				{
//					printf("2 yes \r\n");
					adjustedFWValue = 370;
					Flag_Direction = 0;  //ֹͣ
					vTaskDelay(pdMS_TO_TICKS(2000));   //�ȴ�2��
					returnCompleteFlag = 1;
					break;
				}
				else   //δת��ָ���Ƕ�
				{
//					printf("2 no \r\n");
					//�ȴ�10s��ʱ
					vTaskDelay(pdMS_TO_TICKS(10));   //�ȴ�10ms
					timeCount2++;
					if(timeCount2 == 1000)   //��ʱ
					{
						WitModuleInit();   //���³�ʼ��IMU
					}
					else if(timeCount2 > 1500)   //����IMUģ��֮��δת��ָ���Ƕ�
					{
						Flag_Direction = 3;  //����
						vTaskDelay(pdMS_TO_TICKS(3000));   //�ȴ�3��
						carMode = 1;  //��ѭ��ģʽ
						Flag_Direction = 1;   //ǰ��
					}
				}
			}
			while(timeCount2 < 1500 && emergencyCut_offFlag != 1);
		}
		
		if(emergencyCut_offFlag == 1)
		{
			memset(turnDirArray, 0, 20);
			memset(turnAngleArray, 0, 20);
			count = 0;    // ����ϰ���Ĵ���
			avoidanceCompleteFlag = 0;   // ���������ɱ�־   
			returnCompleteFlag = 0;    // �������·����ɱ�־		
		}
	}
	#endif
	
	#if 0
	static uint8_t firstTurn = 0;
	static float firstTurnAngle = 0.0f;
		
	// �����ȴ������¼�����ʱ100ms����ֹ���п�ʱһֱ������
	static uint8_t obstacleAvoidanceFlag = 0;

//printf("--->%f,%f,%f,%f,%f\r\n",ultrasonic_Receive[0].distance, ultrasonic_Receive[1].distance, ultrasonic_Receive[2].distance, ultrasonic_Receive[3].distance, ultrasonic_Receive[4].distance);

	uint8_t dir = detectObstacle(Measure_Distance[0], Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4]);

	if(dir > 0)
	{
//					printf("<----------------->%d\r\n",Flag_Direction);
		Flag_Direction = dir;
		obstacleAvoidanceFlag = 1;
		trackAndObstacleAvoidanceFlag = 2;
		firstTurn = Flag_Direction;
		firstTurnAngle = currentFWValue;
	}
	else if(dir == 0 && trackAndObstacleAvoidanceFlag == 2 && obstacleAvoidanceFlag == 0)
	{
		trackAndObstacleAvoidanceFlag = 1;
		Flag_Direction = 1;
	}
	
	// ����ϰ����Ƿ����
	if (fabs(currentFWValue - adjustedFWValue) <= 2.0 && obstacleAvoidanceFlag == 1 && trackAndObstacleAvoidanceFlag == 2) {
//		printf("fst.turnAngle = %.2f\r\n",fst.turnAngle);
//		printf("currentFWValue = %.2f, adjustedFWValue = %.2f\r\n",currentFWValue, adjustedFWValue);
		// �л�����·��״̬
		if(Flag_Direction != 3)
			Flag_Direction = 1;
		
		if(fabs(fst.turnAngle) > 60)
		{
			vTaskDelay(pdMS_TO_TICKS(3000));  /* �����ʱ 2�� */
		}
		else
		{
			vTaskDelay(pdMS_TO_TICKS(2000));  /* �����ʱ 1�� */
		}
				
		if(firstTurnAngle == -1)
			return;
		uint8_t count = 0;
		
		switch(firstTurn)
		{
			case 2:
				Flag_Direction = 4;
				
				do
				{
					vTaskDelay(pdMS_TO_TICKS(100));
					count++;
				}while(fabs(currentFWValue - firstTurnAngle) > 2.0 && count < 200 && Flag_Direction != 0);
				
				firstTurnAngle = -1;
				break;
			case 4:
				Flag_Direction = 2;
			
				do
				{
					vTaskDelay(pdMS_TO_TICKS(100));
					count++;
				}while(fabs(currentFWValue - firstTurnAngle) > 2.0 && count < 200 && Flag_Direction != 0);
				
				firstTurnAngle = -1;
				break;
		} 
		
		if(count >= 200)
		{
			trackAndObstacleAvoidanceFlag = 1;
			Flag_Direction = 0;
			return;
		}

//		vTaskDelay(pdMS_TO_TICKS(2000));   //��ʱ1�룬�ȴ��������ٴβ�����Χ�Ƿ����ϰ���
		obstacleAvoidanceFlag = 0;
	}
	#endif
	#else   /* �򵥱��� */
	static uint8_t obstacleCount = 0;

	uint8_t dir = detectObstacle(Measure_Distance[0], Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4]);
	
	if(dir == 0)   // �����ϰ���ֹͣ
	{	
		if(obstacleCount == 0)
		{
			dirLast = Flag_Direction;
			carModeLast = carMode;
		}
		
		Flag_Direction = 0;
		carMode = 2;
		obstacleCount++;
	}
	else
	{
		if(obstacleCount > 0)
		{
			obstacleCount = 0;
		}
		Flag_Direction = dirLast;
		carMode = carModeLast;
	}
	#endif
	#endif
}


void sendDataToAppTask(void)
{
	char sendBuf[20] = {0};
	while(1)
	{
		sprintf(sendBuf, "%d,%d,%d", Flag_Direction, carMode, travelTime);
		Bluetooth_USART_Send_Str((uint8_t *)sendBuf);
		vTaskDelay(pdMS_TO_TICKS(2000));                           /* �����ʱ2000ms */
	}
}

void receiveHallSersonGPSDataTask(void)
{
	gpsUnion gpsUnionArray[2] = {0};
	hallSensorUnion hallSensorUnionArray[2] = {0};	
	
	while (1) {
        // �ȴ��¼���־����ʱ�¼��򴮿ڽ����¼���
        EventBits_t xBits = xEventGroupWaitBits(
            xEventGroup,             // �¼���־��
            UART_RX_F1_EVENT_BIT | UART_RX_F2_EVENT_BIT, // �ȴ����¼�
            pdTRUE,                  // �˳�ʱ�����־λ
            pdFALSE,                 // ����Ҫ�ȴ������¼�
            portMAX_DELAY);          // ���޵ȴ�
        
        // ����Ƿ��Ǵ��ڽ����¼�
        if ((xBits & UART_RX_F1_EVENT_BIT) || (xBits & UART_RX_F2_EVENT_BIT)) {
            // ������յ�������
						{
							if(xBits & UART_RX_F2_EVENT_BIT)
							{
								uint8_t tempBuf[15] = {0};
								memcpy(tempBuf, bigCar_ReceiveDataBuf, 15);
								
//								printf("%s\r\n",tempBuf);
								
								bigCarMotorDataUnpack(0, hallSensorUnionArray, gpsUnionArray, tempBuf);
//								printf("gpsUnionArray --> %d, %d",gpsUnionArray[0].gps, gpsUnionArray[1].gps);

								double GPSDataBuf[2] = {0};
								GPSDataBuf[0] = (double)gpsUnionArray[0].gps/10000000;
								GPSDataBuf[1] = (double)gpsUnionArray[1].gps/10000000;
								printf("hallSensorUnionArray --> %d, %d",hallSensorUnionArray[0].hallSensor, hallSensorUnionArray[1].hallSensor);
								printf("gpsUnionArray --> %lf, %lf",GPSDataBuf[0], GPSDataBuf[1]);
								xQueueSend(xCollectGPSDataQueue, GPSDataBuf, 0);
							}
							else
							{
								uint8_t tempBuf[7] = {0};
								memcpy(tempBuf, bigCar_ReceiveDataBuf, 7);
//								printf("%s\r\n",tempBuf);
								bigCarMotorDataUnpack(1, hallSensorUnionArray, gpsUnionArray, tempBuf);
					
//								printf("hallSensorUnionArray --> %d, %d",hallSensorUnionArray[0].hallSensor, hallSensorUnionArray[1].hallSensor);
							}
						}
//						printf("\r\n");
        }
    }
}

void sendPWMToBigCarTask(void)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(80);  // 80ms����
		
	while(1)
	{
		sendDataToBigCar(); // ��������
		
		// �ȴ���һ������
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

// ѭ��ǰ�ɼ�GPS��
void collectGPSDataTask(void)
{
	double collectGPSDataBuf[2] = {0};
	while(1)
	{
		// �����ȴ�������Ϣ�����õȴ���
		if (xQueueReceive(xCollectGPSDataQueue, collectGPSDataBuf, portMAX_DELAY) == pdTRUE) 
		{
			manual_Collect(collectGPSDataBuf); //���ɼ���GPS���ݴ���Flash��
		}
	}
}

// ����key���ź�������
void receiveKeyDataTask(void)
{
	static uint8_t ON_OFF_Flag = 0;   //�򿪻�رղɼ������־
	
	// �����������Ĵ��������񣨰�ȫ������
	TaskHandle_t collectGPSDataTask_Handle = NULL;
	
	while(1)
	{
		// �ȴ��жϷ��͵��ź���,���û���ź�����������״̬
		if (xSemaphoreTake(xCreateTaskSemaphore, portMAX_DELAY) == pdTRUE) {
			if(ON_OFF_Flag == 0)
			{
//				printf("�����ɹ�\r\n");
				ON_OFF_Flag = 1;
				collectTaskStartFlag = 1;
				taskENTER_CRITICAL();           /* �����ٽ��� */
				/* �������� */
				xTaskCreate((TaskFunction_t )collectGPSDataTask,		// ��������
						(const char*    )"collectGPSDataTask",		// ��������
						(uint16_t       )256,											// ջ��С
						(void*          )NULL,  									// �������
						(UBaseType_t    )7,												// �������ȼ�
						(TaskHandle_t*  )&collectGPSDataTask_Handle); // ������

				taskEXIT_CRITICAL();            /* �˳��ٽ��� */
			}
			else if(ON_OFF_Flag == 1)
			{
//				printf("ɾ���ɹ�\r\n");
				ON_OFF_Flag = 0;
				collectTaskStartFlag = 0;
				#if COLLECT_GPS_MODE    // �ֶ�ģʽ
				double temp[2] = {0};
				manual_Collect(temp);
				#else    // �Զ�ģʽ
				Flag_Direction = 0;
				carMode = 20;
				#endif
				/* ɾ������ */
				taskENTER_CRITICAL();           /* �����ٽ��� */
				if(collectGPSDataTask_Handle != NULL)
					vTaskDelete(collectGPSDataTask_Handle);  /* ɾ�����͵��󳵵��������� */
				taskEXIT_CRITICAL();            /* �˳��ٽ��� */
				collectGPSDataTask_Handle = NULL;
			}
		}
	}
}

/**
 * ����ת����������ʻʱ��
 * @param theta_deg��ת��Ƕȣ��ȣ���30��60��90��
 * @param obstacle_distance���ϰ����ʼ���루�ף�����������ֵ��
 * @param obstacle_width���ϰ����ȣ��ף���ѡ��Ĭ��0��
 * @return ��ʻʱ�䣨���룩
 */
uint16_t CalculateRunTimeAfterTurn(float theta_deg, float obstacle_distance, float obstacle_width) 
{
    // �Ƕ�ת��Ϊ���ȣ���ѧ�⺯���軡���ƣ�
    float theta_rad = theta_deg * PI / 180.0f;
    
    // ����cos��Ϊ0��90��ʱ�����³������,��Ϊ80��֮�����ܴ�
    if (theta_deg >= 80.0f - 1e-6f) { 
        // 90��ת��ʱ�����Ƶ��ϰ�����棬ʱ�䰴�ϰ����ȼ���
        return (uint16_t)(2000);
    }
    
    // ����cos�ȣ�ȷ��ֵ����
    float cos_theta = cos(theta_rad);
    if (cos_theta < 1e-6f) cos_theta = 1e-6f;
    
    // ����������ʻ���루���ϰ����Ⱥ����ࣩ
    float required_distance = (obstacle_distance + 0.2f + obstacle_width/2.0f) / cos_theta;
    
    // ����ʱ�䣨���룩������ȡ��
    return (uint16_t)(required_distance / (RC_Velocity/400.0f) * 1000 + 0.5f);
}


#if defined(PID_ASSISTANT_EN)
void receivePIDDataTask(void)
{
	while(1)
	{
		/* �������ݴ��� */
		receiving_process();	
		int temp = pid0.current_point;    // ��λ����Ҫ����������ת��һ��
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &temp, 1);     // ��ͨ�� 1 ����ʵ��ֵ
		
		vTaskDelay(pdMS_TO_TICKS(100));   
	}
}
#endif

#if GETDISMODE

/**
 * @brief       ����豸
 * @param       dev     : �豸
 *              iic_addr: �豸IICͨѶ��ַ
 * @retval      0: �豸����
 *              1: �豸����
 */
static uint8_t demo_detect_device(uint8_t dir)
{
	uint16_t module_id = 0;

	/* ��ȡ�豸ģ��ID */
	VL53L1_RdWord(dir, &dev, 0x010F, &module_id);
	if (module_id != ATK_MS53L1M_MODULE_ID)
	{
//		printf("ATK-MS53L1M [%d] Detect Failed!\r\n",dir);
		return 1;
	}
			
//	printf("ATK-MS53L1M [%d] ID: 0x%04x\r\n",dir, module_id);

	return 0;		
}

/**
 * @brief       �����豸
 * @param       dev: �豸
 * @retval      0: ���óɹ�
 *              1: ����ʧ��
 */
static uint8_t demo_config_device(uint8_t dir)
{
    VL53L1_Error ret = VL53L1_ERROR_NONE;
    
    /* �豸������� */ 
    ret = VL53L1_WaitDeviceBooted(dir, &dev);
    
    /* �豸һ���Գ�ʼ�� */
    ret = VL53L1_DataInit(dir, &dev);
    
    /* �豸������ʼ�� */
    ret = VL53L1_StaticInit(&dev);
    
    /* �����豸����ģʽ */
    ret = VL53L1_SetDistanceMode(&dev, VL53L1_DISTANCEMODE_LONG);
    
    /* �趨��������ʱ�� */
    ret = VL53L1_SetMeasurementTimingBudgetMicroSeconds(&dev, 40 * 1000);
    
    /* PER��Χ[1ms, 1000ms],���β��֮��ĵȴ�ʱ�� */
    ret = VL53L1_SetInterMeasurementPeriodMilliSeconds(&dev, 50);
    
    /* ֹͣ���� */
    ret = VL53L1_StopMeasurement(dir, &dev);
    
    /* ��ʼ���� */
    ret = VL53L1_StartMeasurement(dir, &dev);
    
    return ret;
}

#endif

