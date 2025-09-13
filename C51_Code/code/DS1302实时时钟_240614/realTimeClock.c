#include <reg51.h>
#include <stdio.h>

#include "DS1302.h"
#include "LCD1602.h"
#include "keyScanAdjustmentTime.h"

uint hexTransformDec(uchar hex);

uchar date[11] = {0};

char code *weekList[] = {"Mon. ", "Tues.", "Wed. ", "Thur.", "Fri. ", "Sta. ", "Sun. "}; 

int main()
{
	uchar dat = 0;
	uchar key = 0;
	
	//�������Ҫ��ʼ��DS1302�����������й����У����¸�λ��ʱ����ʾ�������ʾ��λ������
	//��Ϊд����ȡ�����ַ������ʱ�����ܶ����˸õ�ַ������λ������
	DS1302Init();  
	lcd1602Init();
	
	while(1)
	{
		key = detectKey();  //���ж��Ƿ��м����£����û�������򲻹�
		if(key == 1)
		{
			adjustmentTime(key);
		}
		else
		{
			readTime();  //��DS1302�Ĵ����ж�ȡʱ�ӣ������ds1302Read������

			rtc[0] = hexTransformDec(ds1302Read[0]);  //��
			rtc[1] = hexTransformDec(ds1302Read[1]); //����
			rtc[2] = hexTransformDec(ds1302Read[2]);  //Сʱ
			rtc[5] = hexTransformDec(ds1302Read[5]);   //����
			rtc[3] = hexTransformDec(ds1302Read[3]);   //��
			rtc[4] = hexTransformDec(ds1302Read[4]);  //�·�
			rtc[6] = hexTransformDec(ds1302Read[6]);  //��
			
			//���л�
			sprintf(date, "20%.2d-%.2d-%.2d", rtc[6], rtc[4],rtc[3]);
			lcdDisplatString(1, 1, date);
			sprintf(date, "%s", weekList[rtc[5]-1]);
			lcdDisplatString(12, 1, date);
			sprintf(date, "%.2d:%.2d:%.2d", rtc[2], rtc[1],rtc[0]);
			lcdDisplatString(1, 2, date);
			
		}

	}
	return 0;
}

/*******************************************************************************
* �� �� ��       : hexTransformDec
* ��������		 		 : ��ʮ������ת����ʮ����
* ��    ��       : hex:ʮ������
* ��    ��    	 : ��
*******************************************************************************/
uint hexTransformDec(uchar hex)
{
	uint dat = 0;
	dat = (hex/16)*10 + (hex&0x0f);
	return dat;
}