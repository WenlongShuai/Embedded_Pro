#include "Track.h"
uint8 NUM_LAST = 0;//�ɼ��ĵ����
POSI_ST target_posi;// ��һ��Ҫ׷�ĵ������

//���Ƶ�
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour){
	posi_dest->x = posi_sour.x;
	posi_dest->y = posi_sour.y;
}

//���Ƕ����Ƶ���0,2*PI��
float rad_format(float angle){
	if(angle > 2*PI){
		return angle - 2*PI;
	}
	if(angle < 0){
		return angle + 2*PI;
	}
	return angle;
}

//���ݵѿ����������õ����ԭ��ĺ��򣬷�Χ��0��2*PI��
float my_tan(POSI_ST posi){
	float angle;
	if(posi.x == 0){//�ȿ����������y������
		if(posi.y >= 0){//y��������
			return PI/2;//90��
		}
		if(posi.y < 0){//y�ĸ�����
			return 1.5*PI;//270��
		}
	}
	angle = atan(posi.y/posi.x);//���㷴����
	if(angle > 0){//һ��������
		if(posi.y > 0){
			return angle;//��һ����
		}
		else if(posi.y < 0){
			return angle + PI;//����������+180��
		}
	}
	else if(angle < 0){//����������
		if(posi.y > 0){
			return angle + PI;//�ڶ����ޣ���ʱ�����м�������Ǹ��ģ���ˣ�180��
		}
		else if(posi.y < 0){
			return angle + 2*PI;//�ڶ������ޣ���ʱ�����м�������Ǹ��ģ���ˣ�360��
		}
	}
	else if(angle == 0){//��x����
		if(posi.x > 0){//������
			return 0;
		}
		if(posi.x < 0){//������
			return PI;
		}
	}
	return 0;
}
float my_direc;//����ĺ��򣬵�λΪ��
float direc_set;//����Ŀ����趨�ĺ���,��λΪ��
float direc_offset;//Ŀ�꺽���������Ĳ�, ����direc_set - my_direc
float distance;//Ŀ����복��ľ��룬��λΪm

//���ڼ���Ŀ�����복��ľ���ƫ��ͽǶ�ƫ����������ȫ�ֱ�����
float GET_OFFSET(POSI_ST posi_target){
	POSI_ST posi,posi_offset;
	float direc_offset_term;//ԭʼ����ƫ��
	float distance_term;//ԭʼ����                                                                                         
//                                                                                                                           
	my_direc = rad_format(INS_angle[0] - PI);//my_direc�ڳ�������ʱӦΪ0������ʵҲû��Ҫ��ֻ��ϰ���˾���ĵѿ�������ϵ��������---------> X
	my_direc = 180.*my_direc/PI;//ת��Ϊ�Ƕȣ���ʵ��һ�����岻�󣬵�ʱֻ��Ϊ�˷���۲�Ƕ��Ƿ�����
    
	GET_MY_POSI(&posi);//��ȡ����ĵѿ�������
    
	posi_offset.x = posi_target.x - posi.x;//��������ƫ��
	posi_offset.y = posi_target.y - posi.y;//��������ƫ��
    
	direc_set = my_tan(posi_offset);//Ŀ�����Գ���ĺ������
	direc_set = 180.*direc_set/PI;
    
	direc_offset_term = direc_set - my_direc;
    
	if(direc_offset_term > 180.){//��0���360��Ľ��紦�õ���direc_offset���ܳ��ִ���360����������Ϊ�˷�����п��ƣ�������������ڣ�-180�㣬180�㣩
		direc_offset_term -=360.;
	}
	if(direc_offset_term < -180.){
		direc_offset_term +=360.;
	}
    
	distance_term = pow((pow(posi_offset.x,2)+pow(posi_offset.y,2)),0.5);//���ɶ���������
    
	if(distance_term < 200){//����С��200����Ϊ������Ч��Ŀ�����ų���������
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
	EEPROM_GET_POSI(&target_posi,0);//��target_posiһ����ֵ
	GET_NUM(&NUM_LAST);//��ȡEEPROM�д洢�ĵ����
}

volatile int mode;
volatile uint16 speed = 0;//�����ٶȸ�����pwmֵ����ΧΪ0-1000����1ms�ж��н���б������
int flag_over = 0;//���������־

//׷�ٺ���
void Track(void){
	int servo_out;//���մ��ݸ������pwmֵ
	int flag = 0;//׷���˵�һ����ʱ����1�������ж��Ƿ������˵�һȦ
	static int i = 1;//���ڱ����켣�ϵĵ�
	static uint8 num_circle;//Ȧ��
    
	GET_OFFSET(target_posi);//��ȡĿ����복��ľ���ͺ����
    
	if(distance <= 8){//����Ŀ������С��8�ף���ʼ׷��һ���㣬�������Խ���ܵ�Խ˿�����Ǿ���Խ��
		i++;//ȡ�¸���
		EEPROM_GET_POSI(&target_posi,i);//��EEPROM�ж�ȡ�¸�������
		flag = 1;//��ʾ�Ѿ�����׷���˵�һ����
	}

	mode = get_mode();//��ȡģʽ���˴�Ӧ��ң�ؿ�������
    
	if(mode == 1){//����
		servo_out = 5*direc_offset;//�˴�ֱ�Ӽ��ñ������ƣ��õ�����������������и�ΪPI���ƣ���߾���
		SERVO_SET(servo_out);//�������
		TIM_SetCompare2(TIM1, speed);//�������
		if(flag == 1 && i == NUM_LAST-1){//�жϵ�һȦ�Ѿ�ֻʣһ����
			num_circle++;//Ȧ��+1
			if(distance <= 8){//׷�������һ����
				i = 0;//��ͷ��ʼ׷��
			}
			if(num_circle == 2){//�����˵ڶ�Ȧ
				flag_over = 1;//�������
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