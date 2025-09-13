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
#include "semphr.h"  // 信号量相关类型和函数的声明

extern uint8_t Flag_Left, Flag_Right, Turn_Flag;

extern uint8_t Flag_Direction;   //功能标志位
uint8_t emergencyCut_offFlag = 1;    //紧急停车标志位

extern float RC_Velocity;

uint8_t triggerObstacleAvoidanceDir = 0;    //触发避障的方向
uint8_t collectTaskStartFlag = 0;   //采集任务开始标志位
volatile static uint8_t greaterThresholdCount = 0;   // 存储连续大于阈值的次数


FUZZY_ST fst;

#if !CARTYPE
extern float motorSpeedLeft;
extern float motorSpeedRight;
#else
extern float bigCarMotorSpeedLeft;
extern float bigCarMotorSpeedRight;
extern rtkUnion rtkUnionArray[2];
extern uint8_t rtkUpdateFlag;
extern uint8_t bigCar_ReceiveDataBuf[BIGCAR_RECEIVE_DATA_LEN];  // 接收大车传过来的数据
#endif

extern float speed_core[SPEED_SUM];

extern nmea_msg movingAverage_gpsx;
double gps_array[2][5];  //处理经纬度滑动平均数组

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

float currentFWValue = 0.0f;  //当前方位值
float adjustedFWValue = 0.0f; //发现障碍物，需要调整的方位值

uint8_t carMode = 0;  // 车的模式  1：循迹  2：避障 3：回归路径   4：采点

#if OBSTACLE_AVOIDANCE_MODE
// 简单避障中保存遇到障碍物前一刻的状态
static uint8_t dirLast = 0;
static uint8_t carModeLast = 0;
#endif

//计算转向后所需的行驶时间
uint16_t CalculateRunTimeAfterTurn(float theta_deg, float obstacle_distance, float obstacle_width);

static uint16_t travelTime = 0;
/******************************************************************************************************/
/*FreeRTOS配置*/
void start_task(void);        /* 任务函数 */
void DISTask(void);											/* 任务函数 */
void IMUTask(void);             				/* 任务函数 */
void getDistanceTask(void);             /* 任务函数 */
void receiveAppDataTask(void);             /* 任务函数 */
void gpsDataParseTask(void);             /* 任务函数 */
void obstacleAvoidanceTask(void);										/* 任务函数 */
void readCSBDataEventTask(void);							/* 任务函数 */
void sendDataToAppTask(void);   							/* 任务函数 */
void sendPWMToBigCarTask(void); 									/* 任务函数 */
void receiveKeyDataTask(void);						/* 任务函数 */
void receiveHallSersonGPSDataTask(void);

void vUltrasonicTimeoutCallback(TimerHandle_t xTimer);    //超声波测距超时回调函数
void vPeriodicTimerCallback(TimerHandle_t xTimer);				//定期执行obstacleAvoidanceTask任务的回调函数


/* 任务名称 */
static const char *taskName[TASK_NUM] = {"start_task", "DISTask", "IMUTask", "obstacleAvoidanceTimerTask",
																				 "readCSBDataEventTask", "getDistanceTask", "receiveAppDataTask", "gpsDataParseTask",
																					"sendDataToAppTask", "sendPWMToBigCarTask", "receiveKeyDataTask", "receiveHallSersonGPSDataTask"};
/* 任务函数 */
static taskPtr taskListFun[TASK_NUM] = {start_task, DISTask, IMUTask, obstacleAvoidanceTask, 
																				readCSBDataEventTask, getDistanceTask, receiveAppDataTask, gpsDataParseTask,
																				sendDataToAppTask, sendPWMToBigCarTask, receiveKeyDataTask, receiveHallSersonGPSDataTask};
/* 任务优先级 */
static uint8_t taskPrio[TASK_NUM] = {0, 5, 28, 30, 29, 25, 22, 15, 6, 27, 7, 26};
/* 任务堆栈大小 单位：字(4Byte) */
static uint16_t taskStkSize[TASK_NUM] = {256, 256, 256, 256, 256, 256, 256, 512, 256, 256, 128, 128};
/* 任务句柄 */
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
/* 任务名称 */
static const char *PIDTaskName = "receivePIDDataTask";
/* 任务函数 */
void receivePIDDataTask(void);
/* 任务优先级 */
static uint8_t PIDTaskPrio = 30;
/* 任务堆栈大小 单位：字(4Byte) */
static uint16_t PIDTaskStkSize = 256;
/* 任务句柄 */
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
#if !OBSTACLE_AVOIDANCE_MODE   /* 模糊避障 */
FUZZY_ST detectObstacle(float leftDistance, float midDistance, float rightDistance, float leftSideDistance, float rightSideDistance)
{
	#if 1
	FUZZY_ST fuzzy_Res;  //存放模糊决策结果
	//右转：2  左转：4 后退：3
	if (midDistance > MID_THRESHOLDVALUE && leftDistance > LEFT_THRESHOLDVALUE && rightDistance > RIGHT_THRESHOLDVALUE)
	{
		//全速前进
//		printf("RC_Velocity --> %.2f\r\n",RC_Velocity);
//		printf("motorSpeedLeft --> %.2f, motorSpeedRight --> %.2f\r\n",motorSpeedLeft, motorSpeedRight);
	
		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); //小车 指定速度前进
		#else	
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight); 	//大车 指定速度前进
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
			vTaskDelay(pdMS_TO_TICKS(100));                           /* 相对延时100ms */

			fuzzy_Res = (FUZZY_ST){
			.fuzzyFlag = 0,
			.speed0 = 0,
			.speed1 = 0,
			.turnDir = 0,
			.turnAngle = 0
			};
		}
		else   // 连续3次大于阈值才认为是障碍物才触发避障
		{
			greaterThresholdCount = 0;
			
			// 判断左、中、右哪个方向触发的避障，计算移动时间时需要的障碍物初始距离。
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
				
			fuzzy_Res = fuzzy_decision(leftDistance/100.0f, midDistance/100.0f, rightDistance/100.0f); //使用模糊逻辑进行决策
			fuzzy_Res.fuzzyFlag = 1;
	//		printf("fuzzy_Res.speed0 = %.2f, fuzzy_Res.speed1 = %.2f\r\n",fuzzy_Res.speed0, fuzzy_Res.speed1);
			
			#if !CARTYPE
			Motor_Set_Speed(fuzzy_Res.speed0, fuzzy_Res.speed1);   //将模糊逻辑决策的结果用到小车电机上
			#else
			bigCarMotor_Set_Speed(fuzzy_Res.speed0, fuzzy_Res.speed1); 	//将模糊逻辑决策的结果用到大车电机上
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
					
			vTaskDelay(pdMS_TO_TICKS(TIME_FUZZY_DELAY)); //延时，让模糊逻辑决策的结果作用一段时间
		}		
	}
	
	return fuzzy_Res;
	#endif
	
	#if 0
	//右转：2  左转：4 后退：3
	if (midDistance > MID_THRESHOLDVALUE && leftDistance > LEFT_THRESHOLDVALUE && rightDistance > RIGHT_THRESHOLDVALUE)
	{
		//全速前进
//		printf("RC_Velocity --> %.2f\r\n",RC_Velocity);
//		printf("motorSpeedLeft --> %.2f, motorSpeedRight --> %.2f\r\n",motorSpeedLeft, motorSpeedRight);

		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); 						//小车 指定速度前进
		#else
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight); 	//大车 指定速度前进
		#endif
		
		return 0;
	}
	else
	{
		fst = fuzzy_decision(leftDistance/100.0f, midDistance/100.0f, rightDistance/100.0f); //使用模糊逻辑进行决策
//		printf("fuzzy_Res.speed0 = %.2f, fuzzy_Res.speed1 = %.2f\r\n",fuzzy_Res.speed0, fuzzy_Res.speed1);
		
		if(Flag_Direction != 0)
		{
			#if !CARTYPE
			Motor_Set_Speed(fst.speed0, fst.speed1);   //将模糊逻辑决策的结果用到小车电机上
			#else
			bigCarMotor_Set_Speed(fst.speed0, fst.speed1); 	//将模糊逻辑决策的结果用到大车电机上
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
		
		vTaskDelay(pdMS_TO_TICKS(TIME_FUZZY_DELAY)); //延时，让模糊逻辑决策的结果作用一段时间
		
		return fst.turnDir;
	}
	#endif
}
#endif
#endif

#if OBSTACLE_AVOIDANCE_MODE  /* 简单避障 */
static uint8_t detectObstacle(float leftDistance, float midDistance, float rightDistance, float leftSideDistance, float rightSideDistance)
{
	if (midDistance <= MID_THRESHOLDVALUE || leftDistance <= LEFT_THRESHOLDVALUE || rightDistance <= RIGHT_THRESHOLDVALUE) //遇到障碍物立马停止
	{
		if(greaterThresholdCount < 3)   
		{
			greaterThresholdCount++;
			vTaskDelay(pdMS_TO_TICKS(100));                           /* 相对延时100ms */
			return 1;
		}
		#if !CARTYPE
		Motor_Set_Speed(0, 0);   //将模糊逻辑决策的结果用到小车电机上
		#else
		bigCarMotor_Set_Speed(0, 0);   //将模糊逻辑决策的结果用到大车电机上
		#endif
		
		return 0;
	}
	else  //没有障碍物以正常速度前进
	{
		greaterThresholdCount = 0;
		#if !CARTYPE
		Motor_Set_Speed(motorSpeedLeft, motorSpeedRight);   //将模糊逻辑决策的结果用到小车电机上
		#else
		bigCarMotor_Set_Speed(bigCarMotorSpeedLeft, bigCarMotorSpeedRight);			//将模糊逻辑决策的结果用到大车电机上
		#endif
		return 1;
	}
}
#endif

/******************************************************************************************************/
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
 void freertos_setStartTask(void)
{	
//	xObstacleAvoidanceQueue = xQueueCreate(5, sizeof(uint8_t));
//	configASSERT(xObstacleAvoidanceQueue != NULL);
	
	if(xGPSSerialCmdQueue == NULL)
	{
		// 创建消息队列（长度为5，存储SerialCmd_t类型），用于接收GPS数据
		xGPSSerialCmdQueue = xQueueCreate(5, HIP_USART_REC_LEN * sizeof(uint8_t));
		configASSERT(xGPSSerialCmdQueue != NULL);
	}

	if(xCreateTaskSemaphore == NULL)
	{
		// 创建信号量,用于接收key被按下的信号量
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
		// 创建消息队列（长度为5，存储SerialCmd_t类型），用于接收APP数据
		xAppSerialCmdQueue = xQueueCreate(5, 20*sizeof(uint8_t));
		configASSERT(xAppSerialCmdQueue != NULL);
	}
	
	if(xEventGroup == NULL)
	{
		// 创建事件标志组,用于向大车发送数据或者接收数据
		xEventGroup = xEventGroupCreate();
		configASSERT(xEventGroup != NULL);
	}
	
	#if !GETDISMODE
	for(int i = 0;i < SENSORNUM;i++)	
	{
		ultrasonicSensors[i].sersor_ID = i+1;
		ultrasonicSensors[i].flag = 0;
		
		// 创建事件组
    ultrasonicSensors[i].event_group = xEventGroupCreate();
		
		//创建队列
		ultrasonicSensors[i].data_queue = xQueueCreate(10, sizeof(ultrasonic_sensor_t));
		
		// 创建超时定时器（带传感器ID参数）
		ultrasonicSensors[i].timeout_timer = xTimerCreate(
				"US_TIM", 
				pdMS_TO_TICKS(50),
				pdFALSE, 
				(void*)&ultrasonicSensors[i], // 传递传感器指针作为ID
				vUltrasonicTimeoutCallback
		);
	}
	#endif
	
	/* 初始化模块 */
	fuzzy_Init();
	Bluetooth_USART_GPIO_Init(115200);
	WitModuleInit();   //初始化IMU 
	hi600_Init();  //hi600D初始化
	oledFixedDataDis();
	bigCarMotorControlInit();   // 初始化大车
	
	xTaskCreate((TaskFunction_t )taskListFun[0],            /* 任务函数 */
							(const char*    )taskName[0],          /* 任务名称 */
							(uint16_t       )taskStkSize[0],        /* 任务堆栈大小 */
							(void*          )NULL,                  /* 传入给任务函数的参数 */
							(UBaseType_t    )taskPrio[0],       /* 任务优先级 */
							(TaskHandle_t*  )taskHandle[0]);   /* 任务句柄 */
	vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void)
{
	BaseType_t xReturned;
	
	taskENTER_CRITICAL();           /* 进入临界区 */
	
	#if WORKMODE
	//创建软件定时器，用于执行obstacleAvoidanceTask任务
	TimerHandle_t xPeriodicTimer = xTimerCreate(
			"PeriodicTimer",                // 定时器名称
			pdMS_TO_TICKS(20), // 周期（转换为时钟节拍）
			pdTRUE,                         // 自动重装载
			NULL,                           // 无参数
			vPeriodicTimerCallback          // 回调函数
	);
	configASSERT(xPeriodicTimer != NULL);
	xTimerStart(xPeriodicTimer, 0);    // 启动定时器
	#endif
	
	for(int i = 1;i < TASK_NUM;i++)
	{
		if(i == 3)   
			continue;   //obstacleAvoidanceTask
		
		#if GETDISMODE
		if(i == 4)
			continue; //超声波读取数据任务
		#endif
		
		#if CARTYPE
		if(i == 9 && GET_BLUETOOTH_STATE_LEVEL() == 0)  //蓝牙处于未连接状态
			continue; 	//这里小车大车都不创建发送数据到驱动板的任务
		#endif
		
		#if WORKMODE   // 其他模式
		if(i == 10)
			continue;
		#else      // 采集模式
		if(i == 2 || i == 3 || i == 4 || i == 5)
			continue;
		#endif	
		
		/* 创建任务 */
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
	//显示
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
							
	//避障
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[3],
//							(const char*    )taskName[3],
//							(uint16_t       )taskStkSize[3],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[3],
//							(TaskHandle_t*  )taskHandle[3]);
							
	//读取超声波数据
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[4],
//							(const char*    )taskName[4],
//							(uint16_t       )taskStkSize[4],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[4],
//							(TaskHandle_t*  )taskHandle[4]);
							
	//触发超声波
	xReturned = xTaskCreate((TaskFunction_t )taskListFun[5],
							(const char*    )taskName[5],
							(uint16_t       )taskStkSize[5],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[5],
							(TaskHandle_t*  )taskHandle[5]);
							
	//接收APP数据
	xReturned = xTaskCreate((TaskFunction_t )taskListFun[6],
							(const char*    )taskName[6],
							(uint16_t       )taskStkSize[6],
							(void*          )NULL,
							(UBaseType_t    )taskPrio[6],
							(TaskHandle_t*  )taskHandle[6]);
							
	//GPS读取数据
//	xReturned = xTaskCreate((TaskFunction_t )taskListFun[7],
//							(const char*    )taskName[7],
//							(uint16_t       )taskStkSize[7],
//							(void*          )NULL,
//							(UBaseType_t    )taskPrio[7],
//							(TaskHandle_t*  )taskHandle[7]);
							

	#endif

	vTaskDelete(StartTask_Handler); /* 删除开始任务 */
	taskEXIT_CRITICAL();            /* 退出临界区 */
}


void DISTask(void)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(150);  // 150ms周期
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
		
//		vTaskDelay(pdMS_TO_TICKS(200));                           /* 相对延时200ms */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* 绝对延时200ms */
	}
}

void IMUTask(void)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10);  // 10ms周期
		
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
    vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* 绝对延时10ms */
	}
}

void getDistanceTask(void)
{
	#if !GETDISMODE
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(12*SENSORNUM);  // 12*SENSORNUM ms周期
	Hcsr04Init();  //超声波初始化
	#else
	int status;
	float sum = 0.0f;
	uint8_t num = 0;
	uint8_t data_ready = 0;
	float laser_array[5][2] = {0.0f};
	
	VL53L1_RangingMeasurementData_t vl53l1_data;
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(200);  // 200ms周期
	
	for(int i = 1;i <= ATK_MS53L1M_NUM;i++)
	{
		atk_ms53l1m_init(i);                        /* 模块初始化 */
		demo_detect_device(i);                      /* 检测设备 */
		demo_config_device(i);                      /* 配置设备 */
//		printf("ATK-MS53L1M [%d] Config Succedded!\r\n",i);
	}

	#endif
		
	while(1)
	{
		#if !GETDISMODE
		for(int i = 0;i < SENSORNUM;i++)
		{				
			// 启动该传感器的超时定时器
			xTimerStart(ultrasonicSensors[i].timeout_timer, pdMS_TO_TICKS(5));
			
			Hcsr04Start(i);	//开启超声波模块测距
			
			vTaskDelay(pdMS_TO_TICKS(12)); // 最大等待12ms（对应约2米）
		}
    vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* 绝对延时 xFrequency ms */
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
				demo_detect_device(i+1);                      /* 检测设备 */
				demo_config_device(i+1);                      /* 配置设备 */
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
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);    /* 绝对延时 xFrequency ms */
		#endif
	}
}

//当超时定时器到期时，说明没有收到回波，此时发送超时消息到队列
void vUltrasonicTimeoutCallback(TimerHandle_t xTimer) 
{
	#if !GETDISMODE
   // 从定时器ID获取传感器指针
   ultrasonic_sensor_t* sensor = (ultrasonic_sensor_t*)pvTimerGetTimerID(xTimer);
	
		// 标记超时状态
    sensor->flag = 0;
    
    // 发送超时事件
    xEventGroupSetBits(sensor->event_group, ULTRASONIC_TIMEOUT_BIT);
    
    // 发送无效数据
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
		// 轮流检查每个传感器的事件
		for (int i = 0; i < SENSORNUM; i++) {
				ultrasonic_sensor_t* sensor = &ultrasonicSensors[i];
				
				// 检查是否有事件（不阻塞）
				uxBits = xEventGroupGetBits(sensor->event_group);
				
				if (uxBits & (ULTRASONIC_DATA_READY_BIT | ULTRASONIC_TIMEOUT_BIT)) {
						// 清除事件位
						xEventGroupClearBits(sensor->event_group, 
								ULTRASONIC_DATA_READY_BIT | ULTRASONIC_TIMEOUT_BIT);
						
						// 从队列获取数据
						if (xQueueReceive(sensor->data_queue, &data, 0) == pdPASS) {
								if (data.distance_cm != 0) {
										// 处理有效数据
										printf("Sensor %d: %.2f cm\n", data.sensor_id, data.distance_cm);
									
										// 将传感器数据更新到距离数组中
//										ultrasonic_Distance[data.sensor_id-1] = data.distance_cm;
									ultrasonic_array[data.sensor_id-1][(n++)%2] = data.distance_cm;
												
//										xQueueSend(xObstacleAvoidanceQueue, &ret, portMAX_DELAY);
								} 
								else {
										// 处理超时
										printf("Sensor %d timeout!\n", data.sensor_id);
//										ultrasonic_Distance[data.sensor_id-1] = 400;
									ultrasonic_array[data.sensor_id-1][(n++)%2] = 200;
									// 重置传感器参数，准备下次测量
											ultrasonicSensors[data.sensor_id-1].flag = 0;
											ultrasonicSensors[data.sensor_id-1].start_time = 0;
											ultrasonicSensors[data.sensor_id-1].overflow_cnt = 0;
											ultrasonicSensors[data.sensor_id-1].time_diff = 0;
											ultrasonicSensors[data.sensor_id-1].end_time = 0;
									
										switch(sensor->sersor_ID)
										{
											case 1:
												TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//设置为上升沿捕获
												break;
											case 2:
												TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//设置为上升沿捕获
												break;
											case 3:
												TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising);		//设置为上升沿捕获
												break;
											case 4:
												TIM_OC1PolarityConfig(TIM9, TIM_ICPolarity_Rising);		//设置为上升沿捕获
												break;
											case 5:
												TIM_OC2PolarityConfig(TIM9, TIM_ICPolarity_Rising);		//设置为上升沿捕获
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
			// 短暂休眠以避免占用过多CPU
			vTaskDelay(pdMS_TO_TICKS(20));
		}
		#endif
}


void receiveAppDataTask(void)
{
	u8 Control_Command_BUF[20] = {0};
	
	while(1)
	{
		// 阻塞等待队列消息（永久等待）
		if (xQueueReceive(xAppSerialCmdQueue, Control_Command_BUF, portMAX_DELAY) == pdTRUE) 
		{
			char *disconnectStr = strstr((char *)Control_Command_BUF, (char *)"+DISC:SUCCESS");
			char *connectStr = strstr((char *)Control_Command_BUF, (char *)"+CONNECTED");

			if((disconnectStr && strlen(disconnectStr) > 12))   //处理断开连接
			{
				Flag_Direction = 0;
				emergencyCut_offFlag = 1;
				memset(Control_Command_BUF, 0, 20);
				
				#if CARTYPE
				/* 删除任务 */
				taskENTER_CRITICAL();           /* 进入临界区 */
				if(*taskHandle[9] != NULL)
				{
					vTaskDelete(*taskHandle[9]);  /* 删除发送到大车的数据任务 */
					*taskHandle[9] = NULL;
				}
				taskEXIT_CRITICAL();            /* 退出临界区 */
				#endif
				continue;
			}
			else if((connectStr && strlen(connectStr) > 9) )    //处理建立连接
			{
				Flag_Direction = 0;
				memset(Control_Command_BUF, 0, 20);
				
				#if CARTYPE
				taskENTER_CRITICAL();           /* 进入临界区 */

				if(*taskHandle[9] == NULL)
				{
					/* 创建任务 */
					xTaskCreate((TaskFunction_t )taskListFun[9],
										(const char*    )taskName[9],
										(uint16_t       )taskStkSize[9],
										(void*          )NULL,
										(UBaseType_t    )taskPrio[9],
										(TaskHandle_t*  )taskHandle[9]);
				}		
				taskEXIT_CRITICAL();            /* 退出临界区 */
				#endif
				continue;
			}
			else if(Control_Command_BUF[0] >= 0x40 && Control_Command_BUF[0] <= 0x5F)    // 车辆运动方向
			{
				Flag_Direction = Control_Command_BUF[0] - 0x40;
				if(Flag_Direction == 5)
				{
					RC_Velocity -= 100.0f;  //减速按键，-100mm/s
				}
				else if(Flag_Direction == 6)
				{
					RC_Velocity += 100.0f; //加速按键，+100mm/s
				}

				#if !CARTYPE
				motorSpeedLeft = motorSpeedRight = RC_Velocity/100.0f;
				#else
				bigCarMotorSpeedLeft = bigCarMotorSpeedRight = RC_Velocity/100.0f;
				#endif
				
				speed_core[0] = (RC_Velocity/100)<=0 ? 1 : RC_Velocity/100.0f;
				speed_core[1] = (RC_Velocity/100 - 1)<=0 ? 1 : RC_Velocity/100.0f - 1;
				
				#if defined(PID_ASSISTANT_EN)
				Motor_Set_Speed(motorSpeedLeft, motorSpeedRight); //全速前进
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
					if(Flag_Direction == 5 || Flag_Direction == 6)  // 如果按下的是加速、减速按键，则直接退出
						continue;
					
					emergencyCut_offFlag = 0;
					#if WORKMODE   // 其他模式
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
		// 阻塞等待队列消息（永久等待）
		if (xQueueReceive(xGPSSerialCmdQueue, HIP_USART_RX_BUF, 1000) == pdTRUE)
		{
			#if 0
			char latBuff[12] = {0};
			char lonBuff[12] = {0};
			char *latStr = strstr((char *)HIP_USART_RX_BUF, "lat:");
			char *lonStr = strstr((char *)HIP_USART_RX_BUF, "lon:");
			
			memcpy(latBuff, latStr+4, 10);
			memcpy(lonBuff, lonStr+4, 11);
			
			char *endptr;  // 用于指向未被转换的部分
			
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
			
			#if !WORKMODE   // 采集模式
			
			#if COLLECT_GPS_MODE  // 手动采集
			if(Flag_Direction == 17)   //APP上发送采集GPS数据按钮
			{
				Flag_Direction = 0;
			#else   //自动采集
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
		
		#if WORKMODE  // 其他模式
		if(carMode == 1)
			Track();//循迹函数
		
		#endif
	}
}

void vPeriodicTimerCallback(TimerHandle_t xTimer)
{
	if(Measure_Distance[1] != 0 && emergencyCut_offFlag != 1)   //防止启动时距离数据为0，触发避障。emergencyCut_offFlag紧急停止
	{
//		obstacleAvoidanceTask();
	}
}
	
	
void obstacleAvoidanceTask(void)
{
	#if WORKMODE
	#if !OBSTACLE_AVOIDANCE_MODE   /* 模糊算法避障 */
	#if 1
	static uint8_t turnDirArray[20] = {0};
	static float turnAngleArray[20] = {0.0f};
	static int8_t count = 0;    //记录遇到障碍物的次数
	static uint8_t avoidanceCompleteFlag = 0;   //避障完成标志   
	static uint8_t returnCompleteFlag = 0;    //回归模式下返回路径完成标志

//printf("--->%f,%f,%f,%f,%f\r\n",ultrasonic_Receive[0].distance, ultrasonic_Receive[1].distance, ultrasonic_Receive[2].distance, ultrasonic_Receive[3].distance, ultrasonic_Receive[4].distance);
	
	FUZZY_ST fuzzy_Temp;    //存放模糊决策的结果
	//根据测量距离获取模糊决策的结果，包括转向角度及方向
	fuzzy_Temp = detectObstacle(Measure_Distance[0], Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4]);
		
	if(fuzzy_Temp.fuzzyFlag)    //前方有障碍物
	{
//		printf("1 adjustedFWValue = %.2f, Flag_Direction = %d\r\n",fuzzy_Temp.turnAngle, fuzzy_Temp.turnDir);
		turnDirArray[count] = fuzzy_Temp.turnDir;   //记录转动前的方向
		turnAngleArray[count] = currentFWValue; //记录转动前的角度
		adjustedFWValue = fuzzy_Temp.turnAngle + currentFWValue; //小车需要转动到的角度
		Flag_Direction = fuzzy_Temp.turnDir;    //小车转动方向
		count++;
		
		carMode = 2;   //打开避障模式
		
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
			if(fabs(currentFWValue - adjustedFWValue) <= 5.0)   //转到指定角度
			{
//				printf("1 yes\r\n");
				adjustedFWValue = 370;
				Flag_Direction = 0;  //停止
				vTaskDelay(pdMS_TO_TICKS(2000));   //等待2秒
				break;
			}
			else   // 未转到指定角度
			{
				//等待10s超时
				vTaskDelay(pdMS_TO_TICKS(10));   //等待10ms
				timeCount++;
				if(timeCount == 1000)   //超时
				{
					WitModuleInit();   //重新初始化IMU
				}
				else if(timeCount > 1500)   //重启IMU模块之后还未转到指定角度
				{
					Flag_Direction = 3;  //后退
					vTaskDelay(pdMS_TO_TICKS(3000));   //等待3秒
					carMode = 1;  //打开循迹模式
					Flag_Direction = 1;   //前进
				}
			}
		}
		while(timeCount < 1500 && emergencyCut_offFlag != 1);
	}
	else if(!fuzzy_Temp.fuzzyFlag && carMode != 1)   //前方没有障碍物
	{
		if(carMode == 2 && avoidanceCompleteFlag != 1)  //避障模式
		{
			Flag_Direction = 1;   //前进
			avoidanceCompleteFlag = 1;
			vTaskDelay(travelTime);   //等待travelTime毫秒
		}
		else if(carMode == 2 && avoidanceCompleteFlag == 1)
		{
			avoidanceCompleteFlag = 0;
			carMode = 3;
		}
		
		if(carMode == 3)   //回归路径模式
		{
//			printf("count=%d\r\n",count);
			
			if(returnCompleteFlag)
			{
				returnCompleteFlag = 0;
				Flag_Direction = 1;
				vTaskDelay(travelTime);   //等待travelTime毫秒
			}
			
			if(--count >= 0)   //转到避障前的位置
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
				carMode = 1; //避障完成，进入循迹模式，Flag_Direction方向值由track循迹决定
				memset(turnAngleArray, 0, 20);
				memset(turnDirArray, 0, 20);
				return;
			}
			
			uint16_t timeCount2 = 0;
			
			do
			{
				if(fabs(currentFWValue - adjustedFWValue) <= 5.0)   //转到指定角度
				{
//					printf("2 yes \r\n");
					adjustedFWValue = 370;
					Flag_Direction = 0;  //停止
					vTaskDelay(pdMS_TO_TICKS(2000));   //等待2秒
					returnCompleteFlag = 1;
					break;
				}
				else   //未转到指定角度
				{
//					printf("2 no \r\n");
					//等待10s超时
					vTaskDelay(pdMS_TO_TICKS(10));   //等待10ms
					timeCount2++;
					if(timeCount2 == 1000)   //超时
					{
						WitModuleInit();   //重新初始化IMU
					}
					else if(timeCount2 > 1500)   //重启IMU模块之后还未转到指定角度
					{
						Flag_Direction = 3;  //后退
						vTaskDelay(pdMS_TO_TICKS(3000));   //等待3秒
						carMode = 1;  //打开循迹模式
						Flag_Direction = 1;   //前进
					}
				}
			}
			while(timeCount2 < 1500 && emergencyCut_offFlag != 1);
		}
		
		if(emergencyCut_offFlag == 1)
		{
			memset(turnDirArray, 0, 20);
			memset(turnAngleArray, 0, 20);
			count = 0;    // 清除障碍物的次数
			avoidanceCompleteFlag = 0;   // 清除避障完成标志   
			returnCompleteFlag = 0;    // 清除返回路径完成标志		
		}
	}
	#endif
	
	#if 0
	static uint8_t firstTurn = 0;
	static float firstTurnAngle = 0.0f;
		
	// 阻塞等待距离事件（超时100ms，防止队列空时一直阻塞）
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
	
	// 检测障碍物是否清除
	if (fabs(currentFWValue - adjustedFWValue) <= 2.0 && obstacleAvoidanceFlag == 1 && trackAndObstacleAvoidanceFlag == 2) {
//		printf("fst.turnAngle = %.2f\r\n",fst.turnAngle);
//		printf("currentFWValue = %.2f, adjustedFWValue = %.2f\r\n",currentFWValue, adjustedFWValue);
		// 切换到回路径状态
		if(Flag_Direction != 3)
			Flag_Direction = 1;
		
		if(fabs(fst.turnAngle) > 60)
		{
			vTaskDelay(pdMS_TO_TICKS(3000));  /* 相对延时 2秒 */
		}
		else
		{
			vTaskDelay(pdMS_TO_TICKS(2000));  /* 相对延时 1秒 */
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

//		vTaskDelay(pdMS_TO_TICKS(2000));   //延时1秒，等待超声波再次测量周围是否有障碍物
		obstacleAvoidanceFlag = 0;
	}
	#endif
	#else   /* 简单避障 */
	static uint8_t obstacleCount = 0;

	uint8_t dir = detectObstacle(Measure_Distance[0], Measure_Distance[1], Measure_Distance[2], Measure_Distance[3], Measure_Distance[4]);
	
	if(dir == 0)   // 遇到障碍物停止
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
		vTaskDelay(pdMS_TO_TICKS(2000));                           /* 相对延时2000ms */
	}
}

void receiveHallSersonGPSDataTask(void)
{
	gpsUnion gpsUnionArray[2] = {0};
	hallSensorUnion hallSensorUnionArray[2] = {0};	
	
	while (1) {
        // 等待事件标志（定时事件或串口接收事件）
        EventBits_t xBits = xEventGroupWaitBits(
            xEventGroup,             // 事件标志组
            UART_RX_F1_EVENT_BIT | UART_RX_F2_EVENT_BIT, // 等待的事件
            pdTRUE,                  // 退出时清除标志位
            pdFALSE,                 // 不需要等待所有事件
            portMAX_DELAY);          // 无限等待
        
        // 检查是否是串口接收事件
        if ((xBits & UART_RX_F1_EVENT_BIT) || (xBits & UART_RX_F2_EVENT_BIT)) {
            // 处理接收到的数据
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
  const TickType_t xFrequency = pdMS_TO_TICKS(80);  // 80ms周期
		
	while(1)
	{
		sendDataToBigCar(); // 发送数据
		
		// 等待下一个周期
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

// 循迹前采集GPS点
void collectGPSDataTask(void)
{
	double collectGPSDataBuf[2] = {0};
	while(1)
	{
		// 阻塞等待队列消息（永久等待）
		if (xQueueReceive(xCollectGPSDataQueue, collectGPSDataBuf, portMAX_DELAY) == pdTRUE) 
		{
			manual_Collect(collectGPSDataBuf); //将采集的GPS数据存入Flash中
		}
	}
}

// 接收key的信号量任务
void receiveKeyDataTask(void)
{
	static uint8_t ON_OFF_Flag = 0;   //打开或关闭采集任务标志
	
	// 在任务上下文创建新任务（安全操作）
	TaskHandle_t collectGPSDataTask_Handle = NULL;
	
	while(1)
	{
		// 等待中断发送的信号量,如果没有信号量则处于阻塞状态
		if (xSemaphoreTake(xCreateTaskSemaphore, portMAX_DELAY) == pdTRUE) {
			if(ON_OFF_Flag == 0)
			{
//				printf("创建成功\r\n");
				ON_OFF_Flag = 1;
				collectTaskStartFlag = 1;
				taskENTER_CRITICAL();           /* 进入临界区 */
				/* 创建任务 */
				xTaskCreate((TaskFunction_t )collectGPSDataTask,		// 新任务函数
						(const char*    )"collectGPSDataTask",		// 任务名称
						(uint16_t       )256,											// 栈大小
						(void*          )NULL,  									// 任务参数
						(UBaseType_t    )7,												// 任务优先级
						(TaskHandle_t*  )&collectGPSDataTask_Handle); // 任务句柄

				taskEXIT_CRITICAL();            /* 退出临界区 */
			}
			else if(ON_OFF_Flag == 1)
			{
//				printf("删除成功\r\n");
				ON_OFF_Flag = 0;
				collectTaskStartFlag = 0;
				#if COLLECT_GPS_MODE    // 手动模式
				double temp[2] = {0};
				manual_Collect(temp);
				#else    // 自动模式
				Flag_Direction = 0;
				carMode = 20;
				#endif
				/* 删除任务 */
				taskENTER_CRITICAL();           /* 进入临界区 */
				if(collectGPSDataTask_Handle != NULL)
					vTaskDelete(collectGPSDataTask_Handle);  /* 删除发送到大车的数据任务 */
				taskEXIT_CRITICAL();            /* 退出临界区 */
				collectGPSDataTask_Handle = NULL;
			}
		}
	}
}

/**
 * 计算转向后所需的行驶时间
 * @param theta_deg：转向角度（度，如30、60、90）
 * @param obstacle_distance：障碍物初始距离（米，传感器测量值）
 * @param obstacle_width：障碍物宽度（米，可选，默认0）
 * @return 行驶时间（毫秒）
 */
uint16_t CalculateRunTimeAfterTurn(float theta_deg, float obstacle_distance, float obstacle_width) 
{
    // 角度转换为弧度（数学库函数需弧度制）
    float theta_rad = theta_deg * PI / 180.0f;
    
    // 避免cosθ为0（90°时）导致除零错误,因为80°之后结果很大
    if (theta_deg >= 80.0f - 1e-6f) { 
        // 90°转向时，需绕到障碍物侧面，时间按障碍物宽度计算
        return (uint16_t)(2000);
    }
    
    // 计算cosθ（确保值合理）
    float cos_theta = cos(theta_rad);
    if (cos_theta < 1e-6f) cos_theta = 1e-6f;
    
    // 计算所需行驶距离（含障碍物宽度和冗余）
    float required_distance = (obstacle_distance + 0.2f + obstacle_width/2.0f) / cos_theta;
    
    // 计算时间（毫秒），向上取整
    return (uint16_t)(required_distance / (RC_Velocity/400.0f) * 1000 + 0.5f);
}


#if defined(PID_ASSISTANT_EN)
void receivePIDDataTask(void)
{
	while(1)
	{
		/* 接收数据处理 */
		receiving_process();	
		int temp = pid0.current_point;    // 上位机需要整数参数，转换一下
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &temp, 1);     // 给通道 1 发送实际值
		
		vTaskDelay(pdMS_TO_TICKS(100));   
	}
}
#endif

#if GETDISMODE

/**
 * @brief       检测设备
 * @param       dev     : 设备
 *              iic_addr: 设备IIC通讯地址
 * @retval      0: 设备无误
 *              1: 设备有误
 */
static uint8_t demo_detect_device(uint8_t dir)
{
	uint16_t module_id = 0;

	/* 获取设备模块ID */
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
 * @brief       配置设备
 * @param       dev: 设备
 * @retval      0: 配置成功
 *              1: 配置失败
 */
static uint8_t demo_config_device(uint8_t dir)
{
    VL53L1_Error ret = VL53L1_ERROR_NONE;
    
    /* 设备启动检测 */ 
    ret = VL53L1_WaitDeviceBooted(dir, &dev);
    
    /* 设备一次性初始化 */
    ret = VL53L1_DataInit(dir, &dev);
    
    /* 设备基础初始化 */
    ret = VL53L1_StaticInit(&dev);
    
    /* 配置设备距离模式 */
    ret = VL53L1_SetDistanceMode(&dev, VL53L1_DISTANCEMODE_LONG);
    
    /* 设定完整测距最长时间 */
    ret = VL53L1_SetMeasurementTimingBudgetMicroSeconds(&dev, 40 * 1000);
    
    /* PER范围[1ms, 1000ms],两次测距之间的等待时间 */
    ret = VL53L1_SetInterMeasurementPeriodMilliSeconds(&dev, 50);
    
    /* 停止测量 */
    ret = VL53L1_StopMeasurement(dir, &dev);
    
    /* 开始测量 */
    ret = VL53L1_StartMeasurement(dir, &dev);
    
    return ret;
}

#endif

