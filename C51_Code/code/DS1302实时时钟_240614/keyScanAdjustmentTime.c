#include "keyScanAdjustmentTime.h"

int rtc[7] = {0};  //记录时钟

uchar lcdPos = 0;  //记录光标位置

uchar dat[6] = {0};   //序列化，将一个数据按照格式转换成一个字符串  存放日期 存放星期 存放时间

//统一字符串长度，为了消除星期字符长度不一致导致液晶显示器上显示残留的字符
static char code *weekList[] = {"Mon. ", "Tues.", "Wed. ", "Thur.", "Fri. ", "Sta. ", "Sun. "}; 

void addSubTime(uchar operate);

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
* 函 数 名       : adjustmentTime
* 函数功能		 	 : 功能键
* 输    入       : key:按下的键值
* 输    出    	 : 无
*******************************************************************************/
void adjustmentTime(uchar key)
{
	uchar count = 0;
	while(key!=4)  //如果KEY4按下，说明确定更改
	{
		switch(key)
		{
			case 1:  //切换光标
				count++;
			//设置光标模式，如果光标右移1格，AC就+1
				lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
			case 2:  //对时钟的各个选项进行+1操作
				addSubTime('+');
				break;
			case 3:  //对时钟的各个选项进行-1操作
				addSubTime('-');
				break;
		}
		key = detectKey();
	}
	lcdPos = 0;
	count = 0; //按下KEY1按键的次数清零，防止下次按下时在从0开始计数
	DS1302AdjustmentTime(rtc);//对DS1302写入时间
}

/*******************************************************************************
* 函 数 名       : addSubTime
* 函数功能		 	   : 按下KEY2和KEY3时，对光标处的数值进行加减
* 输    入       : operate:选项，'+':表示需要对光标处的数字进行+1操作   '-':表示需要对光标处的数字进行-1操作 
* 输    出    	 : 无
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
		
		//下面两条语句的作用：因为上面LCD显示完成后，光标会往后移动1格，
		//为了可以一直对这个选项进行更改，那么显示完成后，又让光标和AC回到显示之前的位置，否则按下一次KEY1只能对更改选项改一次。
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		//下面两条语句的作用：因为上面LCD显示完成后，光标会往后移动1格，
		//为了可以一直对这个选项进行更改，那么显示完成后，又让光标和AC回到显示之前的位置，否则按下一次KEY1只能对更改选项改一次。
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
		lcd1602WriteCommand(0x86);
		
	}
	else if(lcdPos == 3)  //day
	{
		if(operate == '+')
		{
			rtc[3]++;
			if(rtc[3] > 28)
			{
				if(rtc[4] == 1 || rtc[4] == 3 || rtc[4] == 5 || rtc[4] == 7 || rtc[4] == 8 || rtc[4] == 10 || rtc[4] == 12)  //大月
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
				if(rtc[4] == 1 || rtc[4] == 3 || rtc[4] == 5 || rtc[4] == 7 || rtc[4] == 8 || rtc[4] == 10 || rtc[4] == 12)  //大月
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
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
		
		lcd1602WriteCommand(0x14);  //设定显示屏或光标移动方向指令
		lcd1602WriteCommand(0xC7);
	}
}


	