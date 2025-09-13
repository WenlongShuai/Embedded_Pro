#include "hip600d.h"
#include <stdlib.h>
#include <string.h>

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 HIP_USART_RX_BUF[HIP_USART_REC_LEN] = {0};     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
u8 HIP_USART_RX_STA_LINE = 0;    //一行数据标志	
u8 HIP_USART_RX_STA_FRAM = 0;   //一帧数据标志
u16 HIP_USART_RX_STA_COUNT = 0;

void hip600d_USART_GPIO_Init(uint32_t bound)
{
	 //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(HIP_USART_RCC_GPIO,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(HIP_USART_RCC,ENABLE);//使能USART3时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(HIP_USART_RX_GPIO, HIP_RX_PinSourcex, HIP_RX_AF); //GPIOB10复用为USART3
	GPIO_PinAFConfig(HIP_USART_TX_GPIO, HIP_TX_PinSourcex, HIP_TX_AF); //GPIOB11复用为USART3
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = HIP_USART_RX_PIN; //GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(HIP_USART_RX_GPIO,&GPIO_InitStructure); //初始化PB11
	
	GPIO_InitStructure.GPIO_Pin = HIP_USART_TX_PIN; //GPIOB10
	GPIO_Init(HIP_USART_TX_GPIO,&GPIO_InitStructure); //初始化PB10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(HIP_USARTx, &USART_InitStructure); //初始化串口3
	
  USART_Cmd(HIP_USARTx, ENABLE);  //使能串口3 
		
	USART_ITConfig(HIP_USARTx, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  NVIC_InitStructure.NVIC_IRQChannel = HIP_USART_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

//解析导航数据
void parseNavigationData(u8 *dataStr) 
{
	u8 dataSize = 0;
	char *endptr;
	u8 splitStrList[20][20] = {0};

	GNRMCStruct rmc;
	
	u8 targetStr[50] = {0};
		
	int res = myFindString(dataStr,(u8 *)"$GNRMC", targetStr);
	if(res == 0)
	{
		//printf("%s\n",targetStr);
		mySplitStr(',', targetStr, splitStrList, &dataSize);
		memcpy(rmc.UTC, splitStrList[1],10); 
	
		rmc.Status = *splitStrList[2];
		
		rmc.Latitude = strtof((char *)splitStrList[3], &endptr); 
		
		rmc.LatitudeDir = *splitStrList[4];
		
		rmc.Longitude = strtof((char *)splitStrList[5], &endptr);
		
		rmc.LongitudeDir = *splitStrList[6];
		
		rmc.SpeedOverGround = strtof((char *)splitStrList[7], &endptr);
		
		rmc.CourseOverGround = strtof((char *)splitStrList[8], &endptr);
		
		memcpy(rmc.Date, splitStrList[9],6); 
		
		rmc.MagneticVariation = strtof((char *)splitStrList[10], &endptr);
		
		rmc.MagneticVariationDir = *splitStrList[11];
		
		rmc.ModeIndicator = *splitStrList[12];
		
		rmc.NavigationalStatus = *splitStrList[13];
		
		printf("UTC时间：%c%c:%c%c:%c%c::%c%c\n",rmc.UTC[0],rmc.UTC[1],rmc.UTC[2],rmc.UTC[3],rmc.UTC[4],rmc.UTC[5],rmc.UTC[7],rmc.UTC[8]);
		printf("定位状态：%d\n",rmc.Status);
		printf("纬度：%f\n",rmc.Latitude);
		printf("纬度方向：%d\n",rmc.LatitudeDir);
		printf("经度：%f\n",rmc.Longitude);
		printf("经度方向：%d\n",rmc.LongitudeDir);
		
		printf("地面速度：%f\n",rmc.SpeedOverGround);
		printf("地面航向：%f\n",rmc.CourseOverGround);
		printf("日期：%c%c.%c%c.%c%c\n",rmc.Date[0],rmc.Date[1],rmc.Date[2],rmc.Date[3],rmc.Date[4],rmc.Date[5]);
		
		printf("磁偏角：%f\n",rmc.MagneticVariation);
		printf("磁偏角方向：%d\n",rmc.MagneticVariationDir);
		
		printf("模式：%c\n",rmc.ModeIndicator);
		printf("航向状态：%c\n",rmc.NavigationalStatus);
	}
}



void mySplitStr(u8 symbol, u8 *dataStr, u8 (*splitStrList)[20], u8 *splitCount)
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

int myFindString(u8 *dataStr, u8 *subStr, u8 *targetStr)
{
	char *result = strstr((char *)dataStr, (char *)subStr);
	
	if(result)
	{
		int i = 0;
		while(*result != 0x0A)
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


