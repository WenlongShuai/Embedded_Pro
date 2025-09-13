#include "keyScanAdjustmentTime.h"

int rtc[7] = {0};  //��¼ʱ��

uchar lcdPos = 0;  //��¼���λ��

uchar dat[6] = {0};   //���л�����һ�����ݰ��ո�ʽת����һ���ַ���  ������� ������� ���ʱ��

//ͳһ�ַ������ȣ�Ϊ�����������ַ����Ȳ�һ�µ���Һ����ʾ������ʾ�������ַ�
static char code *weekList[] = {"Mon. ", "Tues.", "Wed. ", "Thur.", "Fri. ", "Sta. ", "Sun. "}; 

void addSubTime(uchar operate);

/*******************************************************************************
* �� �� ��       : detectKey
* ��������		 		 : �����������Ƿ��м�����
* ��    ��       : ��
* ��    ��    	 : ���ض��������ļ�ֵ
*******************************************************************************/
uchar detectKey()
{
	uchar flag = 0;
	if(!KEY1)
	{
		delayMs(5);  //��������
		while(KEY1);
		flag = 1;
		delayMs(5);  //�ͷ�����
		while(!KEY1);
	}
	else if(!KEY2)
	{
		delayMs(5);
		while(KEY2);
		flag = 2;
		delayMs(5);
		while(!KEY2);	
	}
	else if(!KEY3)
	{
		delayMs(5);
		while(KEY3);
		flag = 3;
		delayMs(5);
		while(!KEY3);
	}
	else if(!KEY4)
	{
		delayMs(5);
		while(KEY4);
		flag = 4;
		delayMs(5);
		while(!KEY4);
	}
	
	return flag;
}


/*******************************************************************************
* �� �� ��       : adjustmentTime
* ��������		 	 : ���ܼ�
* ��    ��       : key:���µļ�ֵ
* ��    ��    	 : ��
*******************************************************************************/
void adjustmentTime(uchar key)
{
	uchar count = 0;
	while(key!=4)  //���KEY4���£�˵��ȷ������
	{
		switch(key)
		{
			case 1:  //�л����
				count++;
			//���ù��ģʽ������������1��AC��+1
				lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
				if(count == 1)
				{
					lcdPos = 1;
					lcd1602WriteCommand(0x83);
				}
				else if(count == 2)
				{
					lcdPos = 2;
					lcd1602WriteCommand(0x86);
				}
				else if(count == 3)
				{
					lcdPos = 3;
					lcd1602WriteCommand(0x89);
				}
				else if(count == 4)
				{
					lcdPos = 4;
					lcd1602WriteCommand(0x8F);
				}
				else if(count == 5)
				{
					lcdPos = 5;
					lcd1602WriteCommand(0xC1);
				}
				else if(count == 6)
				{
					lcdPos = 6;
					lcd1602WriteCommand(0xC4);					
				}
				else if(count == 7)
				{
					lcdPos = 7;
					lcd1602WriteCommand(0xC7);					
					count = 0;
				}
				break;
			case 2:  //��ʱ�ӵĸ���ѡ�����+1����
				addSubTime('+');
				break;
			case 3:  //��ʱ�ӵĸ���ѡ�����-1����
				addSubTime('-');
				break;
		}
		key = detectKey();
	}
	lcdPos = 0;
	count = 0; //����KEY1�����Ĵ������㣬��ֹ�´ΰ���ʱ�ڴ�0��ʼ����
	DS1302AdjustmentTime(rtc);//��DS1302д��ʱ��
}

/*******************************************************************************
* �� �� ��       : addSubTime
* ��������		 	   : ����KEY2��KEY3ʱ���Թ�괦����ֵ���мӼ�
* ��    ��       : operate:ѡ�'+':��ʾ��Ҫ�Թ�괦�����ֽ���+1����   '-':��ʾ��Ҫ�Թ�괦�����ֽ���-1���� 
* ��    ��    	 : ��
*******************************************************************************/
void addSubTime(uchar operate)
{
	if(lcdPos == 1)  //year
	{
		if(operate == '+')
		{
			rtc[6]++;
			if(rtc[6] == 100)
			{
				rtc[6] = 0;
			}
		}
		else
		{
			rtc[6]--;
			if(rtc[6] == -1)
			{
				rtc[6] = 100;
			}
		}
		sprintf(dat,"20%.2d",rtc[6]);
		lcdDisplatString(1,1,dat);
		
		//���������������ã���Ϊ����LCD��ʾ��ɺ󣬹��������ƶ�1��
		//Ϊ�˿���һֱ�����ѡ����и��ģ���ô��ʾ��ɺ����ù���AC�ص���ʾ֮ǰ��λ�ã�������һ��KEY1ֻ�ܶԸ���ѡ���һ�Ρ�
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x83);
		
	}
	else if(lcdPos == 2)  //month
	{
		if(operate == '+')
		{
			rtc[4]++;
			if(rtc[4] == 13)
			{
				rtc[4] = 1;
			}
		}
		else
		{
			rtc[4]--;
			if(rtc[4] == 0)
			{
				rtc[4] = 12;
			}
		}
		sprintf(dat,"%.2d",rtc[4]);
		lcdDisplatString(6,1,dat);
		
		//���������������ã���Ϊ����LCD��ʾ��ɺ󣬹��������ƶ�1��
		//Ϊ�˿���һֱ�����ѡ����и��ģ���ô��ʾ��ɺ����ù���AC�ص���ʾ֮ǰ��λ�ã�������һ��KEY1ֻ�ܶԸ���ѡ���һ�Ρ�
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x86);
		
	}
	else if(lcdPos == 3)  //day
	{
		if(operate == '+')
		{
			rtc[3]++;
			if(rtc[3] > 28)
			{
				if(rtc[4] == 1 || rtc[4] == 3 || rtc[4] == 5 || rtc[4] == 7 || rtc[4] == 8 || rtc[4] == 10 || rtc[4] == 12)  //����
				{
					if(rtc[3] == 32)
					{
						rtc[3] = 1;
					}
				}
				else if(rtc[4] == 4 || rtc[4] == 6 || rtc[4] == 9 || rtc[4] == 11)
				{
					if(rtc[3] == 31)
					{
						rtc[3] = 1;
					}
				}
				else
				{
					if(rtc[3] == 30)
					{
						rtc[3] = 1;
					}
				}
			}
		}	
		else
		{
			rtc[3]--;
			if(rtc[3] == 0)
			{
				if(rtc[4] == 1 || rtc[4] == 3 || rtc[4] == 5 || rtc[4] == 7 || rtc[4] == 8 || rtc[4] == 10 || rtc[4] == 12)  //����
				{
					rtc[3] = 31;
				}
				else if(rtc[4] == 4 || rtc[4] == 6 || rtc[4] == 9 || rtc[4] == 11)
				{
					rtc[3] = 30;
				}
				else
				{
					rtc[3] = 29;
				}
			}
		}
		sprintf(dat,"%.2d",rtc[3]);
		lcdDisplatString(9,1,dat);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x89);
	}
	else if(lcdPos == 4)  //week
	{
		if(operate == '+')
		{
			rtc[5]++;
			if(rtc[5] == 8)
			{
				rtc[5] = 1;
			}
		}
		else
		{
			rtc[5]--;
			if(rtc[5] == 0)
			{
				rtc[5] = 7;
			}
		}
		sprintf(dat,"%s",weekList[rtc[5]-1]);
		lcdDisplatString(12,1,dat);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x8F);
	}
	else if(lcdPos == 5)  //hour
	{
		if(operate == '+')
		{
			rtc[2]++;
			if(rtc[2] == 24)
			{
				rtc[2] = 0;
			}
		}
		else
		{
			rtc[2]--;
			if(rtc[2] == -1)
			{
				rtc[2] = 23;
			}
		}
		sprintf(dat,"%.2d",rtc[2]);
		lcdDisplatString(1,2,dat);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xC1);
	}
	else if(lcdPos == 6)  //minute
	{
		if(operate == '+')
		{
			rtc[1]++;
			if(rtc[1] == 60)
			{
				rtc[1] = 0;
			}
		}
		else
		{
			rtc[1]--;
			if(rtc[1] == -1)
			{
				rtc[1] = 59;
			}
		}
		sprintf(dat,"%.2d",rtc[1]);
		lcdDisplatString(4,2,dat);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xC4);
	}
	else if(lcdPos == 7) //second
	{
		if(operate == '+')
		{
			rtc[0]++;
			if(rtc[0] == 60)
			{
				rtc[0] = 0;
			}
		}
		else
		{
			rtc[0]--;
			if(rtc[0] == -1)
			{
				rtc[0] = 59;
			}
		}
		sprintf(dat,"%.2d",rtc[0]);
		lcdDisplatString(7,2,dat);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xC7);
	}
}


	