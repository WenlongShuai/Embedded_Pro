#include <reg51.h>
#include <stdio.h>

#include "LCD1602.h"

//独立按键
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

uint timer0Count = 0;  //定时器T0计数
int hour = 23;  //小时
int minute = 59; //分钟
int second = 0;  //秒
uint month = 1;  //月份
uint day = 30;   //天
char i = 0;   //星期

//统一字符串长度，为了消除星期字符长度不一致导致液晶显示器上显示残留的字符
char code *weekList[] = {"Mon. ", "Tues.", "Wed. ", "Thur.", "Fri. ", "Sta. ", "Sun. "}; 

char date[6] = {0};   //序列化，将一个数据按照格式转换成一个字符串  存放日期
char week[6] = {0};   //存放星期
char time[9] = {0};		//存放时间

uchar detectKey();   
void detectTime();
void addSubTime(uchar operate);
void timer0Init();

int main()
{
	uchar key = 0;  //独立按键的键值
	uchar count = 0;  //按下KEY1按键的次数

	P3 = 0xff;  
	timer0Init();  //初始化定时器0
	
	lcd1602Init();
	lcdDisplatString(1,1,"Da:");
	lcdDisplatString(9,1,"We:");
	lcdDisplatString(1,2,"time:");

	while(1)
	{
		key = detectKey();  //先判断是否有键按下，如果没键按下则不管
		if(key != 0)  
		{
			TR0 = 0;  //如果有1 2 3 4 的按下则关闭定时器T0，防止在更改时钟的时候，程序进入中断，等待更改完成之后再开启定时器T0
			while(key != 4)  //如果KEY4按下，说明确定更改
			{
				switch(key)
				{
					case 1:  //切换光标
						count++;
					//设置光标模式，如果光标右移1格，AC就+1
						lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
					case 2:  //对时钟的各个选项进行+1操作
						addSubTime('+');
						break;
					case 3:  //对时钟的各个选项进行-1操作
						addSubTime('-');
						break;
				}
				key = detectKey();
			}
			count = 0; //按下KEY1按键的次数清零，防止下次按下时在从0开始计数
			TR0 = 1;  //打开定时器T0
		} 
		
		sprintf(date,"%.2d:%.2d",month,day);  //序列化
		lcdDisplatString(4,1,date);
		
		sprintf(week,"%s",weekList[i]);
		lcdDisplatString(12,1,week);
		
		sprintf(time,"%.2d:%.2d:%.2d",hour,minute,second);
		lcdDisplatString(6,2,time);
	}
	return 0;
}

/*******************************************************************************
* 函 数 名       : timer0Init
* 函数功能		 		 : 初始化定时器T0
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void timer0Init()
{
	//打开总中断
	EA = 1;
	
	//打开定时器0的中断允许控制位
	ET0 = 1;
	
	//设置定时器0的工作方式
	TMOD = 0x01;
	
	//启动定时器0
	TCON = 0x10;  //TR0 = 1
	
	//设置定时器初值
	TH0 = (65536 - 50000) / 256;  //50ms
	TL0 = (65536 - 50000) % 256;

}


/*******************************************************************************
* 函 数 名       : timer0
* 函数功能		 		 : 定时器T0中断服务函数
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : detectTime
* 函数功能		 		 : 根据日常的时间规则来检查时间
* 输    入       : 无
* 输    出    	 : 无
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
					if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //大月
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
* 函 数 名       : detectKey
* 函数功能		 		 : 检测独立按键是否有键按下
* 输    入       : 无
* 输    出    	 : 返回独立按键的键值
*******************************************************************************/
uchar detectKey()
{
	uchar flag = 0;
	if(!KEY1)
	{
		delayMs(5);  //按下消抖
		while(KEY1);
		flag = 1;
		delayMs(5);  //释放消抖
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
* 函 数 名       : addSubTime
* 函数功能		 		 : 对时钟需要更改的选项进行更改
* 输    入       : operate:选项，+操作或者-操作
* 输    出    	 : 无
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
		
		//下面两条语句的作用：因为上面LCD显示完成后，光标会往后移动1格，
		//为了可以一直对这个选项进行更改，那么显示完成后，又让光标和AC回到显示之前的位置，否则按下一次KEY1只能对更改选项改一次。
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
		lcd1602WriteCommand(0x84);
		
	}
	else if(ac == 0x07)  //day
	{
		if(operate == '+')
		{
			day++;
			if(day > 28)
			{
				if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //大月
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
				if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)  //大月
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
		lcd1602WriteCommand(0xCC);
	}
}
