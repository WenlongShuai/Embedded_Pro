#include "hi600.h"
#include <stdlib.h>
#include <string.h>
#include "delay.h"
#include "W25Qxx.h"

#include "FreeRTOS.h"
#include "queue.h"

// 声明全局消息队列句柄
QueueHandle_t xGPSSerialCmdQueue = NULL;

u8 HIP_USART_RX_BUF[HIP_USART_REC_LEN] = {0};     // 接收缓冲,最大USART_REC_LEN个字节.

//接收状态
u8 HIP_USART_RX_STA_FRAM = 0;   // 一帧数据标志
u16 HIP_USART_RX_STA_COUNT = 0; // 接收数据计数

// NMEA 0183 协议解析后数据存放结构体变量
nmea_msg movingAverage_gpsx = {0};

//hi600波特率
const u8 *baudrateBuffer[] = {(const u8 *)"$PAIR864,0,0,9600*13", (const u8 *)"$PAIR864,0,0,57600*28", (const u8 *)"$PAIR864,0,0,115200*1B", 
															(const u8 *)"$PAIR864,0,0,256000*1D", (const u8 *)"$PAIR864,0,0,921600*10"};

//hi600输出速率
const u8 *outRateBuffer[] = {(const u8 *)"$PAIR050,1000*12", (const u8 *)"$PAIR050,500*26", (const u8 *)"$PAIR050,250*24", 
														(const u8 *)"$PAIR050,200*21", (const u8 *)"$PAIR050,100*22"};

// 循迹过程中，将循迹范围的中心GPS转换成笛卡尔坐标系的XY轴
double home_x,home_y;

//根据指定字符进行分割，分割后的数据存入splitStrList数组中
static void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount)
{
	u8 pos = 0,count = 0;
	u8 *temp = dataStr;
	while(*temp != 0x0A)   //换行符
	{
		if(*temp != symbol)
		{
			splitStrList[count][pos] = *temp;
			pos++;
		}
		else 
		{
			pos = 0;
			count++;
		}
		temp++;
	}
	
	*splitCount = count+1;
}

//从字符串中查找字符串，将查找的字符串放入targetStr
//返回值，0：查找成功   1：未查找成功
static int myFindString(u8 *dataStr, u8 *subStr, u8 *targetStr)
{
	char *result = strstr((char *)dataStr, (char *)subStr);
	
	if(result && strlen(result) > 6)
	{
		int i = 0;
		while(*result != '\0')
		{
			targetStr[i] = *result;
			i++;
			result++;
		}
		targetStr[i] = *result;
		return 0;
	}
		
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		世界时间转换为北京时间	
// @param		*time           保存的时间
// @return		void           
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static void utc_to_btc (nmea_msg *time)
{
	uint8_t day_num;
    
	time->utc.hour = time->utc.hour + 8;
	if(time->utc.hour > 23)
	{
		time->utc.hour -= 24;
		time->utc.date += 1;

		if(2 == time->utc.month)
		{
			day_num = 28;
			if((time->utc.year % 4 == 0 && time->utc.year % 100 != 0) || time->utc.year % 400 == 0) // 判断是否为闰年 
			{
				day_num++;                                                     // 闰月 2月为29天
			}
		}
		else
		{
			day_num = 31;                                                       // 1 3 5 7 8 10 12这些月份为31天
			if(4 == time->utc.month || 6  == time->utc.month || 9  == time->utc.month || 11 == time->utc.month )
			{
				day_num = 30;
			}
		}
        
		if(time->utc.date > day_num)
		{
			time->utc.date = 1;
			time->utc.month++;
			if(time->utc.month > 12)
			{
				time->utc.month -= 12;
				time->utc.year++;
			}
		}
	}
}

static void hi600_USART_GPIO_Init(uint32_t bound)
{
	 //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(HIP_USART_RCC_GPIO,ENABLE); // 使能GPIO时钟
	RCC_APB1PeriphClockCmd(HIP_USART_RCC,ENABLE);			 // 使能USART时钟

	// USART端口配置
  GPIO_InitStructure.GPIO_Pin = HIP_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 上拉
	GPIO_Init(HIP_USART_RX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = HIP_USART_TX_PIN;
	GPIO_Init(HIP_USART_TX_GPIO,&GPIO_InitStructure);
	
	// 对应引脚复用映射
	GPIO_PinAFConfig(HIP_USART_RX_GPIO, HIP_RX_PinSourcex, HIP_RX_AF); // GPIO复用为USART
	GPIO_PinAFConfig(HIP_USART_TX_GPIO, HIP_TX_PinSourcex, HIP_TX_AF); // GPIO复用为USART

  // USART初始化设置
	USART_InitStructure.USART_BaudRate = bound;				// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收发模式
  USART_Init(HIP_USARTx, &USART_InitStructure); // 初始化串口
	
	USART_ITConfig(HIP_USARTx, USART_IT_RXNE, ENABLE);// 开启相关中断
	
	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = HIP_USART_IRQn;// 串口中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;// 抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	// 根据指定的参数初始化VIC寄存器
	
	USART_Cmd(HIP_USARTx, ENABLE);  // 使能串口
}

void hi600_Init(void)
{
	hi600_USART_GPIO_Init(115200);
	
//	USART2_Send_BufferData((const u8 *)"$PAIR062,0,0*3E\r\n", sizeof("$PAIR062,0,0*3E\r\n"));		//禁止GGA输出
//	USART2_Send_BufferData((const u8 *)"$PAIR062,2,0*3C\r\n", sizeof("$PAIR062,2,0*3C\r\n"));		//禁止GSA输出
//	USART2_Send_BufferData((const u8 *)"$PAIR062,3,0*3D\r\n", sizeof("$PAIR062,3,0*3D\r\n"));		//禁止GSV输出
//	USART2_Send_BufferData((const u8 *)"$PAIR062,5,0*3B\r\n", sizeof("$PAIR062,5,0*3B\r\n"));		//禁止VTG输出
//	USART2_Send_BufferData((const u8 *)"$PAIR513*3D\r\n", sizeof("$PAIR513*3D\r\n"));						//保存所有配置
//	USART2_Send_BufferData((const u8 *)"$PAIR023*3B\r\n", sizeof("$PAIR023*3B\r\n"));						//重启模块
}

// 初始化循迹中的home坐标,将Flash中的最后一个点当成home的GPS
void trackCenterCoordinatesInit(void)
{
	double temp[2];
	uint8_t num = 0;
	POSI_UNION posi_home;
	
	Get_Num(&num);
	printf("num=%d\r\n",num);
	

	SPI_FLASH_BufferRead(posi_home.data, FLASH_SECTORx(1)+100+(num-1)*sizeof(WGS84_T), sizeof(WGS84_T));
	
	GeodeticToCartesian(posi_home.wgs84.longitude, posi_home.wgs84.latitude, temp);
	
	printf("posi_home.wgs84.longitude=%f,posi_home.wgs84.latitude=%f\r\n",posi_home.wgs84.longitude, posi_home.wgs84.latitude);
	
	home_x = temp[0];
	home_y = temp[1];
}

// 初始化采集过程中的home坐标
void collectCenterCoordinatesInit(void)
{
	home_x = 0;
	home_y = 0;
}



// 分析GPGSV信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 slx=0;
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;

	mySplitStr(',', buf, splitStrList, &dataSize);
	
	gpsx->svnum = strtol((char *)splitStrList[1], &endptr, 10); // 得到可见卫星总数
	gpsx->slmsg[slx].num = strtol((char *)splitStrList[4], &endptr, 10);	// 得到卫星编号
	gpsx->slmsg[slx].eledeg = strtof((char *)splitStrList[5], &endptr);// 得到卫星仰角 
	gpsx->slmsg[slx].azideg = strtof((char *)splitStrList[6], &endptr);// 得到卫星方位角
	gpsx->slmsg[slx].sn = strtof((char *)splitStrList[7], &endptr);	// 得到卫星信噪比
}
// 分析BDGSV信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_BDGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 slx = 0;
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
		
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->beidou_svnum = strtol((char *)splitStrList[3], &endptr, 10) ; // 得到可见卫星总数
	gpsx->beidou_slmsg[slx].beidou_num = strtol((char *)splitStrList[4], &endptr, 10);	// 得到卫星编号
	gpsx->beidou_slmsg[slx].beidou_eledeg = strtof((char *)splitStrList[5], &endptr);// 得到卫星仰角 
	gpsx->beidou_slmsg[slx].beidou_azideg = strtof((char *)splitStrList[6], &endptr);// 得到卫星方位角
	gpsx->beidou_slmsg[slx].beidou_sn = strtof((char *)splitStrList[7], &endptr);	// 得到卫星信噪比
}
// 分析GNGGA信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->gpssta = strtol((char *)splitStrList[6], &endptr, 10); // 得到GPS状态
	gpsx->posslnum = strtol((char *)splitStrList[7], &endptr, 10);	// 得到用于定位的卫星数
	gpsx->altitude = strtof((char *)splitStrList[9], &endptr); // 得到海拔高度
}
// 分析GNGSA信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->fixmode = strtol((char *)splitStrList[2], &endptr, 10); // 得到定位类型
	 
	for(int i = 0;i < 12;i++)
	{
		gpsx->possl[i] = strtol((char *)splitStrList[3+i], &endptr, 10);	// 得到定位卫星编号
	}
	
	gpsx->pdop = strtof((char *)splitStrList[15], &endptr); // 得到PDOP位置精度因子
	gpsx->hdop = strtof((char *)splitStrList[16], &endptr); // 得到HDOP位置精度因子
	gpsx->vdop = strtof((char *)splitStrList[17], &endptr); // 得到VDOP位置精度因子
}
// 分析GNRMC信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	double temp = 0;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	
	// 得到UTC时间,去掉ms
	gpsx->utc.hour = strtol((char *)splitStrList[1], &endptr, 10)/10000;
	gpsx->utc.min = (strtol((char *)splitStrList[1], &endptr, 10)/100)%100;
	gpsx->utc.sec = strtol((char *)splitStrList[1], &endptr, 10)%100;	
	
	//得到纬度	 
	gpsx->latitude = strtod((char *)splitStrList[3], &endptr);	//得到°  mmnn.nnnnn
	temp = gpsx->latitude - (int)(gpsx->latitude/100)*100;   //得到'				 
	gpsx->latitude = (int)(gpsx->latitude/100) + temp/60.0;//转换为°
	
	gpsx->nshemi = *splitStrList[4]; ////南纬还是北纬 
	
	//得到经度	  
	gpsx->longitude = strtod((char *)splitStrList[5], &endptr);;	//得到°
	temp = gpsx->longitude - (int)(gpsx->longitude/100)*100;		//得到'		 
	gpsx->longitude = (int)(gpsx->longitude/100) + temp/60.0;//转换为°
	
	gpsx->ewhemi = *splitStrList[6];		//东经还是西经
	
	//得到UTC日期
	gpsx->utc.date = strtol((char *)splitStrList[9], &endptr, 10)/10000;
	gpsx->utc.month = (strtol((char *)splitStrList[9], &endptr, 10)/100)%100;
	gpsx->utc.year = 2000+strtol((char *)splitStrList[9], &endptr, 10)%100;	
}

// 分析GNVTG信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->speed = strtof((char *)splitStrList[7], &endptr);//得到地面速率
}

// 提取NMEA-0183信息
// gpsx:nmea信息结构体
// buf:接收到的GPS数据缓冲区首地址
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 targetStr[128] = {0};
//	if(!myFindString(buf,(u8 *)"$GPGSV", targetStr))
//	{
//		NMEA_GPGSV_Analysis(gpsx,targetStr);	//GPGSV解析
//	}
	
//	if(!myFindString(buf,(u8 *)"$BDGSV", targetStr))
//	{
//		NMEA_BDGSV_Analysis(gpsx,targetStr);	//BDGSV解析
//	}
	
//	if(!myFindString(buf,(u8 *)"$GNGGA", targetStr))
//	{
//		NMEA_GNGGA_Analysis(gpsx,targetStr);	//GNGGA解析 
//	}
	
//	if(!myFindString(buf,(u8 *)"$GNGSA", targetStr))
//	{
//		NMEA_GNGSA_Analysis(gpsx,targetStr);	//GPNSA解析
//	}
	
	if(!myFindString(buf,(u8 *)"$GNRMC", targetStr))
	{
		NMEA_GNRMC_Analysis(gpsx, targetStr);	//GPNMC解析
	}
	
//	if(!myFindString(buf,(u8 *)"$GNVTG", targetStr))
//	{
//		NMEA_GNVTG_Analysis(gpsx,targetStr);	//GPNTG解析
//	}
}



//根据经纬度解算笛卡尔坐标，存放于temp数组
//高斯-克吕格投影
void GeodeticToCartesian(double longitude,double latitude,double temp[])
{
	double b;//纬度度数
	double L;//经度度数
	double L0;//中央经线度数
	double L1;//L - L0
	double t;//tanB
	double m;//ltanB
	double N;//卯酉圈曲率半径
	double q2;
	double X;// 高斯平面纵坐标
	double Y;// 高斯平面横坐标
	double s;// 赤道至纬度B的经线弧长
	double f; // 参考椭球体扁率
	double e2;// 椭球第一偏心率
	double a; // 参考椭球体长半轴
	//
	double a1;
	double a2;
	double a3;
	double a4;
	double b1;
	double b2;
	double b3;
	double b4;
	double c0;
	double c1;
	double c2;
	double c3;
	//
	int Datum, prjno, zonewide;
	double IPI;
	//
	Datum = 84;// 投影基准面类型：北京54基准面为54，西安80基准面为80，WGS84基准面为84
	prjno = 0;// 投影带号
	zonewide = 3;  //投影带，3°带划分中央子午线 120°E；6°带划分中央子午线 117°E
	IPI = 0.0174532925199433333333; // 3.1415926535898/180.0
	b = latitude; //纬度
	L = longitude;//经度
	if (zonewide == 6)
	{
		prjno = trunc(L / zonewide) + 1;
		L0 = prjno * zonewide - 3;
	}
	else
	{
		prjno = trunc((L - 1.5) / 3) + 1;
		L0 = prjno * 3;
	}
	if (Datum == 54)
	{
		a = 6378245;
		f = 1 / 298.3;
	}
	else if (Datum == 84)
	{
		a = 6378137;
		f = 1 / 298.257223563;
	}
	//
	L0 = L0 * IPI;
	L = L * IPI;
	b = b * IPI;
 
	e2 = 2 * f - f * f; // (a*a-b*b)/(a*a);
	L1 = L - L0;
	t = tan(b);
	m = L1 * cos(b);
	N = a / sqrt(1 - e2 * sin(b) * sin(b));
	q2 = e2 / (1 - e2) * cos(b) * cos(b);
	a1 = 1 + 3 / 4 * e2 + 45 / 64 * e2 * e2 + 175 / 256 * e2 * e2 * e2 + 11025 /
		16384 * e2 * e2 * e2 * e2 + 43659 / 65536 * e2 * e2 * e2 * e2 * e2;
	a2 = 3 / 4 * e2 + 15 / 16 * e2 * e2 + 525 / 512 * e2 * e2 * e2 + 2205 /
		2048 * e2 * e2 * e2 * e2 + 72765 / 65536 * e2 * e2 * e2 * e2 * e2;
	a3 = 15 / 64 * e2 * e2 + 105 / 256 * e2 * e2 * e2 + 2205 / 4096 * e2 * e2 *
		e2 * e2 + 10359 / 16384 * e2 * e2 * e2 * e2 * e2;
	a4 = 35 / 512 * e2 * e2 * e2 + 315 / 2048 * e2 * e2 * e2 * e2 + 31185 /
		13072 * e2 * e2 * e2 * e2 * e2;
	b1 = a1 * a * (1 - e2);
	b2 = -1 / 2 * a2 * a * (1 - e2);
	b3 = 1 / 4 * a3 * a * (1 - e2);
	b4 = -1 / 6 * a4 * a * (1 - e2);
	c0 = b1;
	c1 = 2 * b2 + 4 * b3 + 6 * b4;
	c2 = -(8 * b3 + 32 * b4);
	c3 = 32 * b4;
	s = c0 * b + cos(b) * (c1 * sin(b) + c2 * sin(b) * sin(b) * sin(b) + c3 * sin
		(b) * sin(b) * sin(b) * sin(b) * sin(b));
	X = s + 1 / 2 * N * t * m * m + 1 / 24 * (5 - t * t + 9 * q2 + 4 * q2 * q2)
		* N * t * m * m * m * m + 1 / 720 * (61 - 58 * t * t + t * t * t * t)
		* N * t * m * m * m * m * m * m;
	Y = N * m + 1 / 6 * (1 - t * t + q2) * N * m * m * m + 1 / 120 *
		(5 - 18 * t * t + t * t * t * t - 14 * q2 - 58 * q2 * t * t)
		* N * m * m * m * m * m;
	Y = Y + 1000000 * prjno + 500000;
	temp[0] = X;
	temp[1] = Y;
}


// 根据经纬度获取以设定的home点为原点的笛卡尔坐标
void Get_POSI(double longitude,double latitude,POSI_ST* posi)
{
	double temp[2];
	GeodeticToCartesian(longitude, latitude, temp);//解算
		
	posi->x = temp[0] - home_x;//计算相对home点的坐标
	posi->y = temp[1] - home_y;//计算相对home点的坐标
}


// 获取车身笛卡尔坐标
 void Get_My_POSI(POSI_ST* posi)
{
	Get_POSI(movingAverage_gpsx.longitude, movingAverage_gpsx.latitude, posi);
//	printf("gpsx.longitude ---> %lf    gpsx.latitude ---> %lf    posi.x ---> %lf    posi.y ---> %lf\r\n",gpsx.longitude,gpsx.latitude,posi->x,posi->y);
} 



// 配置SkyTra_GPS/北斗模块波特率
// baud_id:0~8，对应波特率,4800/9600/19200/38400/57600/115200/230400/460800/921600	  
// 返回值:0,执行成功;其他,执行失败(这里不会返回0了)
void hi600_Config_Baudrate(u32 baud_id)
{
	switch(baud_id)
	{
		case 9600:
			USART2_Send_BufferData(baudrateBuffer[0], strlen((const char *)baudrateBuffer[0]));
			break;
		case 57600:
			USART2_Send_BufferData(baudrateBuffer[1], strlen((const char *)baudrateBuffer[1]));
			break;
		case 115200:
			USART2_Send_BufferData(baudrateBuffer[2], strlen((const char *)baudrateBuffer[2]));
			break;
		case 256000:
			USART2_Send_BufferData(baudrateBuffer[3], strlen((const char *)baudrateBuffer[3]));
			break;
		case 921600:
			USART2_Send_BufferData(baudrateBuffer[4], strlen((const char *)baudrateBuffer[4]));
			break;
	}
	
	USART2_Send_BufferData((const u8 *)"$PAIR513*3D\r\n", sizeof("$PAIR513*3D\r\n"));
	USART2_Send_BufferData((const u8 *)"$PAIR023*3B\r\n", sizeof("$PAIR023*3B\r\n"));
} 

// 配置SkyTraF8-BD的更新速率    
// Frep:（取值范围:1,2,4,5,8,10,20,25,40,50）测量时间间隔，单位为Hz，最大不能大于50Hz
// 返回值:0,发送成功;其他,发送失败.
void hi600_Config_OutRate(u8 Frep)
{
	switch(Frep)
	{
		case 1:
			USART2_Send_BufferData(outRateBuffer[0], strlen((const char *)outRateBuffer[0]));
			break;
		case 2:
			USART2_Send_BufferData(outRateBuffer[1], strlen((const char *)outRateBuffer[1]));
			break;
		case 4:
			USART2_Send_BufferData(outRateBuffer[2], strlen((const char *)outRateBuffer[2]));
			break;
		case 5:
			USART2_Send_BufferData(outRateBuffer[3], strlen((const char *)outRateBuffer[3]));
			break;
		case 10:
			USART2_Send_BufferData(outRateBuffer[4], strlen((const char *)outRateBuffer[4]));
			break;
	}
	
	USART2_Send_BufferData((const u8 *)"$PAIR003*39\r\n", sizeof("$PAIR003*39\r\n"));
	USART2_Send_BufferData((const u8 *)"$PAIR513*3D\r\n", sizeof("$PAIR513*3D\r\n"));
	USART2_Send_BufferData((const u8 *)"$PAIR023*3B\r\n", sizeof("$PAIR023*3B\r\n"));
}

void USART2_IRQHandler()
{
	//0x0a:换行符   0x0d:回车符
	u8 Res;	
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(USART_GetITStatus(HIP_USARTx, USART_IT_RXNE) != RESET)  // 接收中断
	{
    Res = USART_ReceiveData(HIP_USARTx);  // 读取接收到的数据

    // 检查缓冲区是否已满
    if(HIP_USART_RX_STA_COUNT < HIP_USART_REC_LEN - 1)
    {
			HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT++] = Res;

			// 判断是否接收到帧结束符 0x0D 0x0A
			//GPS
			#if 1
			if(Res == 0x0A && HIP_USART_RX_STA_COUNT >= 2 && HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT-2] == 0x0D)
			{
				HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT] = '\0';
				
				if(xGPSSerialCmdQueue != NULL)
				{
					xQueueSendFromISR(xGPSSerialCmdQueue, HIP_USART_RX_BUF, &xHigherPriorityTaskWoken);
				}	
				
				HIP_USART_RX_STA_FRAM++;
				HIP_USART_RX_STA_COUNT = 0;
			}
			#endif
			
			#if 0
			//RTK
			if(HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT-2] == 'q')
			{
				HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT] = '\0';				
				xQueueSendFromISR(xGPSSerialCmdQueue, HIP_USART_RX_BUF, &xHigherPriorityTaskWoken);
				
				HIP_USART_RX_STA_COUNT = 0;
				memset(HIP_USART_RX_BUF, 0, HIP_USART_REC_LEN);
			}
			#endif
    }
    else
    {
        // 缓冲区已满，重置接收
        HIP_USART_RX_STA_COUNT = 0;
    }
    
    USART_ClearITPendingBit(HIP_USARTx, USART_IT_RXNE);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void USART2_Send_BufferData(const u8 *buffer, u16 len)
{
	for(int i = 0;i < len;i++)
	{		
		USART_SendData(HIP_USARTx, buffer[i]);
		while(USART_GetFlagStatus(HIP_USARTx, USART_FLAG_TXE) == RESET);
	}
}


