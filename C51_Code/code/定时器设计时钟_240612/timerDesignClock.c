#include <reg51.h>
#include <stdio.h>

#include "LCD1602.h"

//��������
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

uint timer0Count = 0;  //��ʱ��T0����
int hour = 23;  //Сʱ
int minute = 59; //����
int second = 0;  //��
uint month = 1;  //�·�
uint day = 30;   //��
char i = 0;   //����

//ͳһ�ַ������ȣ�Ϊ�����������ַ����Ȳ�һ�µ���Һ����ʾ������ʾ�������ַ�
char code *weekList[] = {"Mon. ", "Tues.", "Wed. ", "Thur.", "Fri. ", "Sta. ", "Sun. "}; 

char date[6] = {0};   //���л�����һ�����ݰ��ո�ʽת����һ���ַ���  �������
char week[6] = {0};   //�������
char time[9] = {0};		//���ʱ��

uchar detectKey();   
void detectTime();
void addSubTime(uchar operate);
void timer0Init();

int main()
{
	uchar key = 0;  //���������ļ�ֵ
	uchar count = 0;  //����KEY1�����Ĵ���

	P3 = 0xff;  
	timer0Init();  //��ʼ����ʱ��0
	
	lcd1602Init();
	lcdDisplatString(1,1,"Da:");
	lcdDisplatString(9,1,"We:");
	lcdDisplatString(1,2,"time:");

	while(1)
	{
		key = detectKey();  //���ж��Ƿ��м����£����û�������򲻹�
		if(key != 0)  
		{
			TR0 = 0;  //�����1 2 3 4 �İ�����رն�ʱ��T0����ֹ�ڸ���ʱ�ӵ�ʱ�򣬳�������жϣ��ȴ��������֮���ٿ�����ʱ��T0
			while(key != 4)  //���KEY4���£�˵��ȷ������
			{
				switch(key)
				{
					case 1:  //�л����
						count++;
					//���ù��ģʽ������������1��AC��+1
						lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
						if(count == 1)
						{
							lcd1602WriteCommand(0x84);
						}
						else if(count == 2)
						{
							lcd1602WriteCommand(0x87);
						}
						else if(count == 3)
						{
							lcd1602WriteCommand(0x8F);
						}
						else if(count == 4)
						{
							lcd1602WriteCommand(0xC6);
						}
						else if(count == 5)
						{
							lcd1602WriteCommand(0xC9);
						}
						else if(count == 6)
						{
							lcd1602WriteCommand(0xCC);					
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
			count = 0; //����KEY1�����Ĵ������㣬��ֹ�´ΰ���ʱ�ڴ�0��ʼ����
			TR0 = 1;  //�򿪶�ʱ��T0
		} 
		
		sprintf(date,"%.2d:%.2d",month,day);  //���л�
		lcdDisplatString(4,1,date);
		
		sprintf(week,"%s",weekList[i]);
		lcdDisplatString(12,1,week);
		
		sprintf(time,"%.2d:%.2d:%.2d",hour,minute,second);
		lcdDisplatString(6,2,time);
	}
	return 0;
}

/*******************************************************************************
* �� �� ��       : timer0Init
* ��������		 		 : ��ʼ����ʱ��T0
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void timer0Init()
{
	//�����ж�
	EA = 1;
	
	//�򿪶�ʱ��0���ж��������λ
	ET0 = 1;
	
	//���ö�ʱ��0�Ĺ�����ʽ
	TMOD = 0x01;
	
	//������ʱ��0
	TCON = 0x10;  //TR0 = 1
	
	//���ö�ʱ����ֵ
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;

}


/*******************************************************************************
* �� �� ��       : timer0
* ��������		 		 : ��ʱ��T0�жϷ�����
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void timer0() interrupt 1
{
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;
	timer0Count++;
	
	if(timer0Count == 20)  //1s
	{
		timer0Count = 0;
		second++;
		if(second == 60)
		{
			detectTime();
		}
	}
}


/*******************************************************************************
* �� �� ��       : detectTime
* ��������		 		 : �����ճ���ʱ����������ʱ��
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void detectTime()
{
	if(second == 60)
	{
		second = 0;
		minute++;
		if(minute == 60)
		{
			minute = 0;
			hour++;
			if(hour == 24)
			{
				hour = 0;
				day++;
				i++;
				if(i == 7)
				{
					i = 0;
				}
				if(day > 28)
				{
					if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //����
					{
						if(day == 32)
						{
							if(month == 12)
							{	
								month = 1;
							}
							else
							{
								month++;
							}
							day = 1;
						}
					}
					else if(month == 4 || month == 6 || month == 9 || month == 11)
					{
						if(day == 31)
						{
							day = 1;
							month++;
						}
					}
					else
					{
						if(day == 30)
						{
							day = 1;
							month++;
						}
					}
				}
			}
		}
	}
}

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
* �� �� ��       : addSubTime
* ��������		 		 : ��ʱ����Ҫ���ĵ�ѡ����и���
* ��    ��       : operate:ѡ�+��������-����
* ��    ��    	 : ��
*******************************************************************************/
void addSubTime(uchar operate)
{
	uchar ac = 0;
	ac = lcd1602ReadState();
	if(ac == 0x04)  //month
	{
		if(operate == '+')
		{
			month++;
			if(month == 13)
			{
				month = 1;
			}
		}
		else
		{
			month--;
			if(month == 0)
			{
				month = 12;
			}
		}
		sprintf(date,"%.2d",month);
		lcdDisplatString(4,1,date);
		
		//���������������ã���Ϊ����LCD��ʾ��ɺ󣬹��������ƶ�1��
		//Ϊ�˿���һֱ�����ѡ����и��ģ���ô��ʾ��ɺ����ù���AC�ص���ʾ֮ǰ��λ�ã�������һ��KEY1ֻ�ܶԸ���ѡ���һ�Ρ�
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x84);
		
	}
	else if(ac == 0x07)  //day
	{
		if(operate == '+')
		{
			day++;
			if(day > 28)
			{
				if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //����
				{
					if(day == 32)
					{
						day = 1;
					}
				}
				else if(month == 4 || month == 6 || month == 9 || month == 11)
				{
					if(day == 31)
					{
						day = 1;
					}
				}
				else
				{
					if(day == 30)
					{
						day = 1;
					}
				}
			}
		}	
		else
		{
			day--;
			if(day == 0)
			{
				if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //����
				{
					day = 31;
				}
				else if(month == 4 || month == 6 || month == 9 || month == 11)
				{
					day = 30;
				}
				else
				{
					day = 29;
				}
			}
		}
		sprintf(date,"%.2d",day);
		lcdDisplatString(7,1,date);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x87);
	}
	else if(ac == 0x0f)  //week
	{
		if(operate == '+')
		{
			i++;
			if(i == 7)
			{
				i = 0;
			}
		}
		else
		{
			i--;
			if(i == -1)
			{
				i = 6;
			}
		}
		sprintf(week,"%s",weekList[i]);
		lcdDisplatString(12,1,week);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0x8F);
	}
	else if(ac == 0x46)  //hour
	{
		if(operate == '+')
		{
			hour++;
			if(hour == 24)
			{
				hour = 0;
			}
		}
		else
		{
			hour--;
			if(hour == -1)
			{
				hour = 23;
			}
		}
		sprintf(time,"%.2d",hour);
		lcdDisplatString(6,2,time);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xC6);
	}
	else if(ac == 0x49)  //minute
	{
		if(operate == '+')
		{
			minute++;
			if(minute == 60)
			{
				minute = 0;
			}
		}
		else
		{
			minute--;
			if(minute == -1)
			{
				minute = 59;
			}
		}
		sprintf(time,"%.2d",minute);
		lcdDisplatString(9,2,time);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xC9);
	}
	else if(ac == 0x4c) //second
	{
		if(operate == '+')
		{
			second++;
			if(second == 60)
			{
				second = 0;
			}
		}
		else
		{
			second--;
			if(second == -1)
			{
				second = 59;
			}
		}
		sprintf(time,"%.2d",second);
		lcdDisplatString(12,2,time);
		
		lcd1602WriteCommand(0x14);  //�趨��ʾ�������ƶ�����ָ��
		lcd1602WriteCommand(0xCC);
	}
}
