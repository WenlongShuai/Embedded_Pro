#include "hi600.h"
#include <stdlib.h>
#include <string.h>
#include "delay.h"
#include "W25Qxx.h"

#include "FreeRTOS.h"
#include "queue.h"

// ����ȫ����Ϣ���о��
QueueHandle_t xGPSSerialCmdQueue = NULL;

u8 HIP_USART_RX_BUF[HIP_USART_REC_LEN] = {0};     // ���ջ���,���USART_REC_LEN���ֽ�.

//����״̬
u8 HIP_USART_RX_STA_FRAM = 0;   // һ֡���ݱ�־
u16 HIP_USART_RX_STA_COUNT = 0; // �������ݼ���

// NMEA 0183 Э����������ݴ�Žṹ�����
nmea_msg movingAverage_gpsx = {0};

//hi600������
const u8 *baudrateBuffer[] = {(const u8 *)"$PAIR864,0,0,9600*13", (const u8 *)"$PAIR864,0,0,57600*28", (const u8 *)"$PAIR864,0,0,115200*1B", 
															(const u8 *)"$PAIR864,0,0,256000*1D", (const u8 *)"$PAIR864,0,0,921600*10"};

//hi600�������
const u8 *outRateBuffer[] = {(const u8 *)"$PAIR050,1000*12", (const u8 *)"$PAIR050,500*26", (const u8 *)"$PAIR050,250*24", 
														(const u8 *)"$PAIR050,200*21", (const u8 *)"$PAIR050,100*22"};

// ѭ�������У���ѭ����Χ������GPSת���ɵѿ�������ϵ��XY��
double home_x,home_y;

//����ָ���ַ����зָ�ָ������ݴ���splitStrList������
static void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount)
{
	u8 pos = 0,count = 0;
	u8 *temp = dataStr;
	while(*temp != 0x0A)   //���з�
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

//���ַ����в����ַ����������ҵ��ַ�������targetStr
//����ֵ��0�����ҳɹ�   1��δ���ҳɹ�
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
// @brief		����ʱ��ת��Ϊ����ʱ��	
// @param		*time           �����ʱ��
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
			if((time->utc.year % 4 == 0 && time->utc.year % 100 != 0) || time->utc.year % 400 == 0) // �ж��Ƿ�Ϊ���� 
			{
				day_num++;                                                     // ���� 2��Ϊ29��
			}
		}
		else
		{
			day_num = 31;                                                       // 1 3 5 7 8 10 12��Щ�·�Ϊ31��
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
	 //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(HIP_USART_RCC_GPIO,ENABLE); // ʹ��GPIOʱ��
	RCC_APB1PeriphClockCmd(HIP_USART_RCC,ENABLE);			 // ʹ��USARTʱ��

	// USART�˿�����
  GPIO_InitStructure.GPIO_Pin = HIP_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // ����
	GPIO_Init(HIP_USART_RX_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = HIP_USART_TX_PIN;
	GPIO_Init(HIP_USART_TX_GPIO,&GPIO_InitStructure);
	
	// ��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(HIP_USART_RX_GPIO, HIP_RX_PinSourcex, HIP_RX_AF); // GPIO����ΪUSART
	GPIO_PinAFConfig(HIP_USART_TX_GPIO, HIP_TX_PinSourcex, HIP_TX_AF); // GPIO����ΪUSART

  // USART��ʼ������
	USART_InitStructure.USART_BaudRate = bound;				// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// �շ�ģʽ
  USART_Init(HIP_USARTx, &USART_InitStructure); // ��ʼ������
	
	USART_ITConfig(HIP_USARTx, USART_IT_RXNE, ENABLE);// ��������ж�
	
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = HIP_USART_IRQn;// �����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;// ��ռ���ȼ�7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	// ����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_Cmd(HIP_USARTx, ENABLE);  // ʹ�ܴ���
}

void hi600_Init(void)
{
	hi600_USART_GPIO_Init(115200);
	
//	USART2_Send_BufferData((const u8 *)"$PAIR062,0,0*3E\r\n", sizeof("$PAIR062,0,0*3E\r\n"));		//��ֹGGA���
//	USART2_Send_BufferData((const u8 *)"$PAIR062,2,0*3C\r\n", sizeof("$PAIR062,2,0*3C\r\n"));		//��ֹGSA���
//	USART2_Send_BufferData((const u8 *)"$PAIR062,3,0*3D\r\n", sizeof("$PAIR062,3,0*3D\r\n"));		//��ֹGSV���
//	USART2_Send_BufferData((const u8 *)"$PAIR062,5,0*3B\r\n", sizeof("$PAIR062,5,0*3B\r\n"));		//��ֹVTG���
//	USART2_Send_BufferData((const u8 *)"$PAIR513*3D\r\n", sizeof("$PAIR513*3D\r\n"));						//������������
//	USART2_Send_BufferData((const u8 *)"$PAIR023*3B\r\n", sizeof("$PAIR023*3B\r\n"));						//����ģ��
}

// ��ʼ��ѭ���е�home����,��Flash�е����һ���㵱��home��GPS
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

// ��ʼ���ɼ������е�home����
void collectCenterCoordinatesInit(void)
{
	home_x = 0;
	home_y = 0;
}



// ����GPGSV��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 slx=0;
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;

	mySplitStr(',', buf, splitStrList, &dataSize);
	
	gpsx->svnum = strtol((char *)splitStrList[1], &endptr, 10); // �õ��ɼ���������
	gpsx->slmsg[slx].num = strtol((char *)splitStrList[4], &endptr, 10);	// �õ����Ǳ��
	gpsx->slmsg[slx].eledeg = strtof((char *)splitStrList[5], &endptr);// �õ��������� 
	gpsx->slmsg[slx].azideg = strtof((char *)splitStrList[6], &endptr);// �õ����Ƿ�λ��
	gpsx->slmsg[slx].sn = strtof((char *)splitStrList[7], &endptr);	// �õ����������
}
// ����BDGSV��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_BDGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 slx = 0;
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
		
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->beidou_svnum = strtol((char *)splitStrList[3], &endptr, 10) ; // �õ��ɼ���������
	gpsx->beidou_slmsg[slx].beidou_num = strtol((char *)splitStrList[4], &endptr, 10);	// �õ����Ǳ��
	gpsx->beidou_slmsg[slx].beidou_eledeg = strtof((char *)splitStrList[5], &endptr);// �õ��������� 
	gpsx->beidou_slmsg[slx].beidou_azideg = strtof((char *)splitStrList[6], &endptr);// �õ����Ƿ�λ��
	gpsx->beidou_slmsg[slx].beidou_sn = strtof((char *)splitStrList[7], &endptr);	// �õ����������
}
// ����GNGGA��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->gpssta = strtol((char *)splitStrList[6], &endptr, 10); // �õ�GPS״̬
	gpsx->posslnum = strtol((char *)splitStrList[7], &endptr, 10);	// �õ����ڶ�λ��������
	gpsx->altitude = strtof((char *)splitStrList[9], &endptr); // �õ����θ߶�
}
// ����GNGSA��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->fixmode = strtol((char *)splitStrList[2], &endptr, 10); // �õ���λ����
	 
	for(int i = 0;i < 12;i++)
	{
		gpsx->possl[i] = strtol((char *)splitStrList[3+i], &endptr, 10);	// �õ���λ���Ǳ��
	}
	
	gpsx->pdop = strtof((char *)splitStrList[15], &endptr); // �õ�PDOPλ�þ�������
	gpsx->hdop = strtof((char *)splitStrList[16], &endptr); // �õ�HDOPλ�þ�������
	gpsx->vdop = strtof((char *)splitStrList[17], &endptr); // �õ�VDOPλ�þ�������
}
// ����GNRMC��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	double temp = 0;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	
	// �õ�UTCʱ��,ȥ��ms
	gpsx->utc.hour = strtol((char *)splitStrList[1], &endptr, 10)/10000;
	gpsx->utc.min = (strtol((char *)splitStrList[1], &endptr, 10)/100)%100;
	gpsx->utc.sec = strtol((char *)splitStrList[1], &endptr, 10)%100;	
	
	//�õ�γ��	 
	gpsx->latitude = strtod((char *)splitStrList[3], &endptr);	//�õ���  mmnn.nnnnn
	temp = gpsx->latitude - (int)(gpsx->latitude/100)*100;   //�õ�'				 
	gpsx->latitude = (int)(gpsx->latitude/100) + temp/60.0;//ת��Ϊ��
	
	gpsx->nshemi = *splitStrList[4]; ////��γ���Ǳ�γ 
	
	//�õ�����	  
	gpsx->longitude = strtod((char *)splitStrList[5], &endptr);;	//�õ���
	temp = gpsx->longitude - (int)(gpsx->longitude/100)*100;		//�õ�'		 
	gpsx->longitude = (int)(gpsx->longitude/100) + temp/60.0;//ת��Ϊ��
	
	gpsx->ewhemi = *splitStrList[6];		//������������
	
	//�õ�UTC����
	gpsx->utc.date = strtol((char *)splitStrList[9], &endptr, 10)/10000;
	gpsx->utc.month = (strtol((char *)splitStrList[9], &endptr, 10)/100)%100;
	gpsx->utc.year = 2000+strtol((char *)splitStrList[9], &endptr, 10)%100;	
}

// ����GNVTG��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 dataSize = 0;
	u8 splitStrList[20][20] = {0};
	char *endptr;
	
	mySplitStr(',', buf, splitStrList, &dataSize);
	gpsx->speed = strtof((char *)splitStrList[7], &endptr);//�õ���������
}

// ��ȡNMEA-0183��Ϣ
// gpsx:nmea��Ϣ�ṹ��
// buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 targetStr[128] = {0};
//	if(!myFindString(buf,(u8 *)"$GPGSV", targetStr))
//	{
//		NMEA_GPGSV_Analysis(gpsx,targetStr);	//GPGSV����
//	}
	
//	if(!myFindString(buf,(u8 *)"$BDGSV", targetStr))
//	{
//		NMEA_BDGSV_Analysis(gpsx,targetStr);	//BDGSV����
//	}
	
//	if(!myFindString(buf,(u8 *)"$GNGGA", targetStr))
//	{
//		NMEA_GNGGA_Analysis(gpsx,targetStr);	//GNGGA���� 
//	}
	
//	if(!myFindString(buf,(u8 *)"$GNGSA", targetStr))
//	{
//		NMEA_GNGSA_Analysis(gpsx,targetStr);	//GPNSA����
//	}
	
	if(!myFindString(buf,(u8 *)"$GNRMC", targetStr))
	{
		NMEA_GNRMC_Analysis(gpsx, targetStr);	//GPNMC����
	}
	
//	if(!myFindString(buf,(u8 *)"$GNVTG", targetStr))
//	{
//		NMEA_GNVTG_Analysis(gpsx,targetStr);	//GPNTG����
//	}
}



//���ݾ�γ�Ƚ���ѿ������꣬�����temp����
//��˹-������ͶӰ
void GeodeticToCartesian(double longitude,double latitude,double temp[])
{
	double b;//γ�ȶ���
	double L;//���ȶ���
	double L0;//���뾭�߶���
	double L1;//L - L0
	double t;//tanB
	double m;//ltanB
	double N;//î��Ȧ���ʰ뾶
	double q2;
	double X;// ��˹ƽ��������
	double Y;// ��˹ƽ�������
	double s;// �����γ��B�ľ��߻���
	double f; // �ο����������
	double e2;// �����һƫ����
	double a; // �ο������峤����
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
	Datum = 84;// ͶӰ��׼�����ͣ�����54��׼��Ϊ54������80��׼��Ϊ80��WGS84��׼��Ϊ84
	prjno = 0;// ͶӰ����
	zonewide = 3;  //ͶӰ����3����������������� 120��E��6����������������� 117��E
	IPI = 0.0174532925199433333333; // 3.1415926535898/180.0
	b = latitude; //γ��
	L = longitude;//����
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


// ���ݾ�γ�Ȼ�ȡ���趨��home��Ϊԭ��ĵѿ�������
void Get_POSI(double longitude,double latitude,POSI_ST* posi)
{
	double temp[2];
	GeodeticToCartesian(longitude, latitude, temp);//����
		
	posi->x = temp[0] - home_x;//�������home�������
	posi->y = temp[1] - home_y;//�������home�������
}


// ��ȡ����ѿ�������
 void Get_My_POSI(POSI_ST* posi)
{
	Get_POSI(movingAverage_gpsx.longitude, movingAverage_gpsx.latitude, posi);
//	printf("gpsx.longitude ---> %lf    gpsx.latitude ---> %lf    posi.x ---> %lf    posi.y ---> %lf\r\n",gpsx.longitude,gpsx.latitude,posi->x,posi->y);
} 



// ����SkyTra_GPS/����ģ�鲨����
// baud_id:0~8����Ӧ������,4800/9600/19200/38400/57600/115200/230400/460800/921600	  
// ����ֵ:0,ִ�гɹ�;����,ִ��ʧ��(���ﲻ�᷵��0��)
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

// ����SkyTraF8-BD�ĸ�������    
// Frep:��ȡֵ��Χ:1,2,4,5,8,10,20,25,40,50������ʱ��������λΪHz������ܴ���50Hz
// ����ֵ:0,���ͳɹ�;����,����ʧ��.
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
	//0x0a:���з�   0x0d:�س���
	u8 Res;	
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(USART_GetITStatus(HIP_USARTx, USART_IT_RXNE) != RESET)  // �����ж�
	{
    Res = USART_ReceiveData(HIP_USARTx);  // ��ȡ���յ�������

    // ��黺�����Ƿ�����
    if(HIP_USART_RX_STA_COUNT < HIP_USART_REC_LEN - 1)
    {
			HIP_USART_RX_BUF[HIP_USART_RX_STA_COUNT++] = Res;

			// �ж��Ƿ���յ�֡������ 0x0D 0x0A
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
        // ���������������ý���
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


