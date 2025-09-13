#include "track.h"

#include "fun.h"

uint8_t NUM_LAST = 0;	// �ɼ��ĵ����
POSI_ST target_posi;	// ��һ��Ҫ׷�ĵ������

float my_direc = 0.0f;// ����ĺ��򣬵�λΪ��
float direc_set = 0.0f;// ����Ŀ����趨�ĺ���,��λΪ��
float direc_offset = 0.0f;// Ŀ�꺽���������Ĳ�, ����direc_set - my_direc
float distance = 0.0f;// Ŀ����복��ľ��룬��λΪm

volatile int mode;
volatile uint16_t speed = 0;// �����ٶȸ�����pwmֵ����ΧΪ0-1000����1ms�ж��н���б������
int flag_over = 0;// ���������־

extern u8 Flag_Direction;
extern volatile float fAcc[3], fGyro[3], fMag[3], fAngle[3];

extern nmea_msg movingAverage_gpsx;
extern POSI_UNION posi_now;

// ��ԴXYλ�ø��Ƹ�Ŀ��XYλ��
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour)
{
	posi_dest->x = posi_sour.x;
	posi_dest->y = posi_sour.y;
}


// ���ݵѿ����������õ����ԭ��ĺ��򣬷�Χ��0��2*PI�������ص��ǻ���
float my_tan(POSI_ST posi)
{
	float angle = 0.0f;
	if(posi.x == 0)
	{
		//�ȿ����������y������
		if(posi.y >= 0)
		{
			//y��������
			return (float)(PI/2.0f);//90��
		}
		if(posi.y < 0)
		{
			//y�ĸ�����
			return (float)(1.5f*PI);//270��
		}
	}
	angle = atan(posi.y/posi.x);//���㷴����
	if(angle > 0)
	{
		//һ��������
		if(posi.y > 0)
		{
			return angle;//��һ����
		}
		else if(posi.y < 0)
		{
			return (double)angle + PI;//����������+180��
		}
	}
	else if(angle < 0)
	{
		//����������
		if(posi.y > 0)
		{
			return (double)angle + PI;//�ڶ����ޣ���ʱ�����м�������Ǹ��ģ���ˣ�180��
		}
		else if(posi.y < 0)
		{
			return (double)angle + 2.0f*PI;//�������ޣ���ʱ�����м�������Ǹ��ģ���ˣ�360��
		}
	}
	else if(angle == 0)
	{
		//��x����
		if(posi.x > 0)
		{
			//������
			return 0;
		}
		if(posi.x < 0)
		{
			//������
			return PI;
		}
	}
	return 0;
}

// �����Ŀ����복��ľ��루С��200Ϊ��Ч���룩��Ŀ����복��ĺ���-180��+180��
float Get_Offset(POSI_ST posi_target)
{
	POSI_ST posi,posi_offset;
	float direc_offset_term;//ԭʼ����ƫ��
	float distance_term;//ԭʼ����   
                                                                                                                    
	my_direc = angle_format(-fAngle[2]-90);  //��ܽǶȴ���360��С��0��ֵ 
	
	Get_My_POSI(&posi);//��ȡ����ĵѿ�������
    
	posi_offset.x = posi_target.x - posi.x;//��������ƫ��
	posi_offset.y = posi_target.y - posi.y;//��������ƫ��
    
	direc_set = my_tan(posi_offset);//Ŀ�����Գ���ĺ������
	direc_set = 180.0f*(double)direc_set/PI;  //����ת���ɶ�
//	 	
//	printf("3333 posi.x --> %lf,posi.y --> %lf\r\n",posi.x, posi.y);

//	printf("3333 posi_target.x --> %lf,posi_target.y --> %lf\r\n",posi_target.x, posi_target.y);

	printf("3333 my_direc --> %lf,direc_set --> %lf\r\n",my_direc, direc_set);

    
	direc_offset_term = (direc_set - my_direc); //Ŀ�������ڳ���ĺ�����ʵʱ�����ĺ������ֵ
    
//	if(direc_offset_term > (float)180.0){//��0���360��Ľ��紦�õ���direc_offset���ܳ��ִ���360����������Ϊ�˷�����п��ƣ�������������ڣ�-180�㣬180�㣩
//		direc_offset_term -= (float)360.0;
//	}
//	if(direc_offset_term < (float)-180.0){
//		direc_offset_term += (float)360.0;
//	}
	    
	distance_term = pow((pow(posi_offset.x, 2)+pow(posi_offset.y, 2)), 0.5);//���ɶ�����㳵����Ŀ���ľ���
    
	printf("direc_offset_term ---> %f\r\n",direc_offset_term);
//	printf("distance_term ---> %f\r\n",distance_term);

	if(distance_term < 200){//����С��200����Ϊ������Ч��Ŀ�����ų���������
		direc_offset = direc_offset_term;
		distance = distance_term;
	}
	return 0;
}


void Track_Init(void)
{
	Flash_Get_POSI(&target_posi, 1);//��target_posiһ����ֵ����ȡĿ����еĵ�һ����
	Get_Num(&NUM_LAST);//��ȡEEPROM�д洢�ĵ����
}


//׷�ٺ���
void Track(void)
{
	int flag = 0;//׷���˵�һ����ʱ����1�������ж��Ƿ������˵�һȦ
	volatile static int i = 2;//���ڱ����켣�ϵĵ�
    
	if(Flag_Direction != 0) //��ȡģʽ���˴�Ӧ��ң�ؿ�������
	{
		Get_Offset(target_posi);//��ȡĿ����복��ľ���ͺ����
//   
//		printf("distance ---> %f\r\n",distance);
//		printf("direc_offset ---> %f\r\n",direc_offset);
		if(distance <= 0.1 || fabs(movingAverage_gpsx.longitude-posi_now.wgs84.longitude)<=0.000001 && fabs(movingAverage_gpsx.latitude-posi_now.wgs84.latitude)<=0.000001)
		{
			if(i < NUM_LAST)
			{
				//����Ŀ������С��8�ף���ʼ׷��һ���㣬�������Խ���ܵ�Խ˿�����Ǿ���Խ��
				Flash_Get_POSI(&target_posi, i);//��EEPROM�ж�ȡ�¸�������
				i++;//ȡ�¸���
			}
			flag = 1;//��ʾ�Ѿ�׷����һ��Ŀ���
		}
		
		//����
		if(direc_offset > 10)  //��ת
		{
			Flag_Direction = 2;
		}
		else if(direc_offset < -10)  //��ת
		{
			Flag_Direction = 4;
		}
		else   //ǰ��
		{
			Flag_Direction = 1;
		}
		
		if(flag == 1 && i == NUM_LAST)
		{
			Flag_Direction = 0; //ͣ��
			return;
		}
	}
	else
	{
		Flag_Direction = 0; //ͣ��
	}
}


