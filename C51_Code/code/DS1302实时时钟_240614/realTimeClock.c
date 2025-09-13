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
	
	//这里必须要初始化DS1302，否则在运行过程中，按下复位键时，显示会出错（显示三位数），
	//因为写命令取读秒地址的数据时，可能读到了该地址的其他位的数据
	DS1302Init();  
	lcd1602Init();
	
	while(1)
	{
		key = detectKey();  //先判断是否有键按下，如果没键按下则不管
		if(key == 1)
		{
			adjustmentTime(key);
		}
		else
		{
			readTime();  //从DS1302寄存器中读取时钟，存放在ds1302Read数组中

			rtc[0] = hexTransformDec(ds1302Read[0]);  //秒
			rtc[1] = hexTransformDec(ds1302Read[1]); //分钟
			rtc[2] = hexTransformDec(ds1302Read[2]);  //小时
			rtc[5] = hexTransformDec(ds1302Read[5]);   //星期
			rtc[3] = hexTransformDec(ds1302Read[3]);   //天
			rtc[4] = hexTransformDec(ds1302Read[4]);  //月份
			rtc[6] = hexTransformDec(ds1302Read[6]);  //年
			
			//序列化
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
* 函 数 名       : hexTransformDec
* 函数功能		 		 : 将十六进制转换成十进制
* 输    入       : hex:十六进制
* 输    出    	 : 无
*******************************************************************************/
uint hexTransformDec(uchar hex)
{
	uint dat = 0;
	dat = (hex/16)*10 + (hex&0x0f);
	return dat;
}