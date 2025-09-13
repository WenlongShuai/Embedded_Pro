#include "Track.h"
uint8 NUM_LAST = 0;//采集的点个数
POSI_ST target_posi;// 下一个要追的点的坐标

//复制点
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour){
	posi_dest->x = posi_sour.x;
	posi_dest->y = posi_sour.y;
}

//将角度限制到（0,2*PI）
float rad_format(float angle){
	if(angle > 2*PI){
		return angle - 2*PI;
	}
	if(angle < 0){
		return angle + 2*PI;
	}
	return angle;
}

//根据笛卡尔坐标计算该点相对原点的航向，范围（0，2*PI）
float my_tan(POSI_ST posi){
	float angle;
	if(posi.x == 0){//先考虑坐标点在y轴的情况
		if(posi.y >= 0){//y的正半轴
			return PI/2;//90°
		}
		if(posi.y < 0){//y的负半轴
			return 1.5*PI;//270°
		}
	}
	angle = atan(posi.y/posi.x);//计算反正切
	if(angle > 0){//一、三象限
		if(posi.y > 0){
			return angle;//第一象限
		}
		else if(posi.y < 0){
			return angle + PI;//第三象限则+180°
		}
	}
	else if(angle < 0){//二、四象限
		if(posi.y > 0){
			return angle + PI;//第二象限，此时反正切计算出来是负的，因此＋180°
		}
		else if(posi.y < 0){
			return angle + 2*PI;//第二四象限，此时反正切计算出来是负的，因此＋360°
		}
	}
	else if(angle == 0){//在x轴上
		if(posi.x > 0){//正半轴
			return 0;
		}
		if(posi.x < 0){//负半轴
			return PI;
		}
	}
	return 0;
}
float my_direc;//车身的航向，单位为°
float direc_set;//根据目标点设定的航向,单位为°
float direc_offset;//目标航向和自身航向的差, 即：direc_set - my_direc
float distance;//目标点与车身的距离，单位为m

//用于计算目标点距离车身的距离偏差和角度偏差，结果保存在全局变量中
float GET_OFFSET(POSI_ST posi_target){
	POSI_ST posi,posi_offset;
	float direc_offset_term;//原始航向偏移
	float distance_term;//原始距离                                                                                         
//                                                                                                                           
	my_direc = rad_format(INS_angle[0] - PI);//my_direc在朝向正东时应为0，但其实也没必要，只是习惯了经典的笛卡尔坐标系的正方向---------> X
	my_direc = 180.*my_direc/PI;//转换为角度，其实这一步意义不大，当时只是为了方便观察角度是否正常
    
	GET_MY_POSI(&posi);//获取车身的笛卡尔坐标
    
	posi_offset.x = posi_target.x - posi.x;//计算坐标偏移
	posi_offset.y = posi_target.y - posi.y;//计算坐标偏移
    
	direc_set = my_tan(posi_offset);//目标点相对车身的航向计算
	direc_set = 180.*direc_set/PI;
    
	direc_offset_term = direc_set - my_direc;
    
	if(direc_offset_term > 180.){//在0°和360°的交界处得到的direc_offset可能出现大于360°的情况，且为了方便进行控制，将航向差限制在（-180°，180°）
		direc_offset_term -=360.;
	}
	if(direc_offset_term < -180.){
		direc_offset_term +=360.;
	}
    
	distance_term = pow((pow(posi_offset.x,2)+pow(posi_offset.y,2)),0.5);//勾股定理计算距离
    
	if(distance_term < 200){//距离小于200米认为数据有效，目的是排除错误数据
		direc_offset = direc_offset_term;
		distance = distance_term;
	}
	return 0;
}

void Track_Show(void){
	char str[10][10];
	POSI_ST posi;
	posi.x = 0;
	posi.y = 0;
	sprintf(str[0],"%.2f",my_direc);
	lcd_showstr(0,2,(char*)str[0]);
	sprintf(str[1],"%.2f",direc_set);
	lcd_showstr(5,3,(char*)str[1]);
	sprintf(str[2],"%.2f",direc_offset);
	lcd_showstr(5,4,(char*)str[2]);
	sprintf(str[3],"%.2f",distance);
	lcd_showstr(5,5,(char*)str[3]);
}



void Track_INIT(void){
	EEPROM_GET_POSI(&target_posi,0);//给target_posi一个初值
	GET_NUM(&NUM_LAST);//获取EEPROM中存储的点个数
}

volatile int mode;
volatile uint16 speed = 0;//开环速度给定的pwm值，范围为0-1000，在1ms中断中进行斜坡启动
int flag_over = 0;//任务结束标志

//追踪函数
void Track(void){
	int servo_out;//最终传递给舵机的pwm值
	int flag = 0;//追到了第一个点时即置1，用来判断是否跑完了第一圈
	static int i = 1;//用于遍历轨迹上的点
	static uint8 num_circle;//圈数
    
	GET_OFFSET(target_posi);//获取目标点与车身的距离和航向差
    
	if(distance <= 8){//距离目标点距离小于8米，则开始追下一个点，这个参数越大跑的越丝滑但是精度越低
		i++;//取下个点
		EEPROM_GET_POSI(&target_posi,i);//从EEPROM中读取下个点坐标
		flag = 1;//表示已经至少追到了第一个点
	}

	mode = get_mode();//获取模式，此处应由遥控控制启动
    
	if(mode == 1){//启动
		servo_out = 5*direc_offset;//此处直接简单用比例控制，得到舵机给定量，可自行改为PI控制，提高精度
		SERVO_SET(servo_out);//给到舵机
		TIM_SetCompare2(TIM1, speed);//给到电机
		if(flag == 1 && i == NUM_LAST-1){//判断第一圈已经只剩一个点
			num_circle++;//圈数+1
			if(distance <= 8){//追到了最后一个点
				i = 0;//从头开始追点
			}
			if(num_circle == 2){//走完了第二圈
				flag_over = 1;//任务完成
				SERVO_SET(0);
				TIM_SetCompare2(TIM1, 0);				
			}
		}
		if(flag_over == 1){
			SERVO_SET(0);
			TIM_SetCompare2(TIM1, 0);
			return;
		}
		
	}
	else{
		SERVO_SET(0);
		TIM_SetCompare2(TIM1, 0);
	}
}