#include "track.h"

#include "fun.h"

uint8_t NUM_LAST = 0;	// 采集的点个数
POSI_ST target_posi;	// 下一个要追的点的坐标

float my_direc = 0.0f;// 车身的航向，单位为°
float direc_set = 0.0f;// 根据目标点设定的航向,单位为°
float direc_offset = 0.0f;// 目标航向和自身航向的差, 即：direc_set - my_direc
float distance = 0.0f;// 目标点与车身的距离，单位为m

volatile int mode;
volatile uint16_t speed = 0;// 开环速度给定的pwm值，范围为0-1000，在1ms中断中进行斜坡启动
int flag_over = 0;// 任务结束标志

extern u8 Flag_Direction;
extern volatile float fAcc[3], fGyro[3], fMag[3], fAngle[3];

extern nmea_msg movingAverage_gpsx;
extern POSI_UNION posi_now;

// 将源XY位置复制给目标XY位置
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour)
{
	posi_dest->x = posi_sour.x;
	posi_dest->y = posi_sour.y;
}


// 根据笛卡尔坐标计算该点相对原点的航向，范围（0，2*PI），返回的是弧度
float my_tan(POSI_ST posi)
{
	float angle = 0.0f;
	if(posi.x == 0)
	{
		//先考虑坐标点在y轴的情况
		if(posi.y >= 0)
		{
			//y的正半轴
			return (float)(PI/2.0f);//90°
		}
		if(posi.y < 0)
		{
			//y的负半轴
			return (float)(1.5f*PI);//270°
		}
	}
	angle = atan(posi.y/posi.x);//计算反正切
	if(angle > 0)
	{
		//一、三象限
		if(posi.y > 0)
		{
			return angle;//第一象限
		}
		else if(posi.y < 0)
		{
			return (double)angle + PI;//第三象限则+180°
		}
	}
	else if(angle < 0)
	{
		//二、四象限
		if(posi.y > 0)
		{
			return (double)angle + PI;//第二象限，此时反正切计算出来是负的，因此＋180°
		}
		else if(posi.y < 0)
		{
			return (double)angle + 2.0f*PI;//第四象限，此时反正切计算出来是负的，因此＋360°
		}
	}
	else if(angle == 0)
	{
		//在x轴上
		if(posi.x > 0)
		{
			//正半轴
			return 0;
		}
		if(posi.x < 0)
		{
			//负半轴
			return PI;
		}
	}
	return 0;
}

// 计算出目标点与车身的距离（小于200为有效距离）和目标点与车身的航向差（-180，+180）
float Get_Offset(POSI_ST posi_target)
{
	POSI_ST posi,posi_offset;
	float direc_offset_term;//原始航向偏移
	float distance_term;//原始距离   
                                                                                                                    
	my_direc = angle_format(-fAngle[2]-90);  //规避角度大于360，小于0的值 
	
	Get_My_POSI(&posi);//获取车身的笛卡尔坐标
    
	posi_offset.x = posi_target.x - posi.x;//计算坐标偏移
	posi_offset.y = posi_target.y - posi.y;//计算坐标偏移
    
	direc_set = my_tan(posi_offset);//目标点相对车身的航向计算
	direc_set = 180.0f*(double)direc_set/PI;  //弧度转换成度
//	 	
//	printf("3333 posi.x --> %lf,posi.y --> %lf\r\n",posi.x, posi.y);

//	printf("3333 posi_target.x --> %lf,posi_target.y --> %lf\r\n",posi_target.x, posi_target.y);

	printf("3333 my_direc --> %lf,direc_set --> %lf\r\n",my_direc, direc_set);

    
	direc_offset_term = (direc_set - my_direc); //目标点相对于车身的航向与实时测量的航向求差值
    
//	if(direc_offset_term > (float)180.0){//在0°和360°的交界处得到的direc_offset可能出现大于360°的情况，且为了方便进行控制，将航向差限制在（-180°，180°）
//		direc_offset_term -= (float)360.0;
//	}
//	if(direc_offset_term < (float)-180.0){
//		direc_offset_term += (float)360.0;
//	}
	    
	distance_term = pow((pow(posi_offset.x, 2)+pow(posi_offset.y, 2)), 0.5);//勾股定理计算车身与目标点的距离
    
	printf("direc_offset_term ---> %f\r\n",direc_offset_term);
//	printf("distance_term ---> %f\r\n",distance_term);

	if(distance_term < 200){//距离小于200米认为数据有效，目的是排除错误数据
		direc_offset = direc_offset_term;
		distance = distance_term;
	}
	return 0;
}


void Track_Init(void)
{
	Flash_Get_POSI(&target_posi, 1);//给target_posi一个初值，获取目标点中的第一个点
	Get_Num(&NUM_LAST);//获取EEPROM中存储的点个数
}


//追踪函数
void Track(void)
{
	int flag = 0;//追到了第一个点时即置1，用来判断是否跑完了第一圈
	volatile static int i = 2;//用于遍历轨迹上的点
    
	if(Flag_Direction != 0) //获取模式，此处应由遥控控制启动
	{
		Get_Offset(target_posi);//获取目标点与车身的距离和航向差
//   
//		printf("distance ---> %f\r\n",distance);
//		printf("direc_offset ---> %f\r\n",direc_offset);
		if(distance <= 0.1 || fabs(movingAverage_gpsx.longitude-posi_now.wgs84.longitude)<=0.000001 && fabs(movingAverage_gpsx.latitude-posi_now.wgs84.latitude)<=0.000001)
		{
			if(i < NUM_LAST)
			{
				//距离目标点距离小于8米，则开始追下一个点，这个参数越大跑的越丝滑但是精度越低
				Flash_Get_POSI(&target_posi, i);//从EEPROM中读取下个点坐标
				i++;//取下个点
			}
			flag = 1;//表示已经追到下一个目标点
		}
		
		//启动
		if(direc_offset > 10)  //右转
		{
			Flag_Direction = 2;
		}
		else if(direc_offset < -10)  //左转
		{
			Flag_Direction = 4;
		}
		else   //前进
		{
			Flag_Direction = 1;
		}
		
		if(flag == 1 && i == NUM_LAST)
		{
			Flag_Direction = 0; //停车
			return;
		}
	}
	else
	{
		Flag_Direction = 0; //停车
	}
}


