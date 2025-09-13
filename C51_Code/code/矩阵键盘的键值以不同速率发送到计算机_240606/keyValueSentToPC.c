#include <reg51.h>
#include <stdio.h>
#include <string.h>

//按下矩阵键盘第一行时以1200bps发送1、2、3、4。
//第二行时以2400bps发送5、6、7、8，
//第三行以4800bps发送9、10、11、12，
//第四行以9600bps发送13、14、15、16。

void delayMs(unsigned int ms);
int keyboardScanf();

int keyNum = 0;
char sentData[3] = {0};
int main()
{
	unsigned char len = 0;
	unsigned char i = 0;
	//设置定时器T1的工作方式以及启动
	TMOD = 0x20;
	TR1 = 1;
	
	//设置串口的工作方式以及启动
	SCON = 0x50;
	PCON = 0x00;  //SMOD = 0
	
	while(1)
	{
		keyNum = keyboardScanf();
		if(keyNum == 0)
		{
			continue;
		}
		else
		{
			switch(keyNum)
			{
				//1200bps
				case 1:
				case 2:
				case 3:
				case 4:
					TH1 = 0xE8;
					TL1 = 0xE8;
					break;
				//2400bps
				case 5:
				case 6:
				case 7:
				case 8:
					TH1 = 0xF4;
					TL1 = 0xF4;
					break;
				//4800bps
				case 9:
				case 10:
				case 11:
				case 12:
					TH1 = 0xFA;
					TL1 = 0xFA;
					break;
				//9600bps
				case 13:
				case 14:
				case 15:
				case 16:
					TH1 = 0xFD;
					TL1 = 0xFD;
					break;
			}
			
			sprintf(sentData, "%d", keyNum);
			len = strlen(sentData);
		  i = 0;
			while(i < len)
			{
				SBUF = sentData[i];
				while(!TI);
				TI = 0;
				i++;
			}
			//memset(sentData, len, 0);  //把数组里面的内容清零
		}
	}
	return 0;
}

int keyboardScanf()
{
	int num = 0;
	P1 = 0x7F;  //μúò?DD
	if(P1 != 0x7F)
	{
		delayMs(5);
		while(P1 == 0x7F);
		
		switch(P1)
		{
			case 0x77:num = 1;break;
			case 0x7B:num = 2;break;
			case 0x7D:num = 3;break;
			case 0x7E:num = 4;break;
		}
		delayMs(5);
		while(P1 != 0x7F);
	}
	
	P1 = 0xBF;  //μú?tDD
	if(P1 != 0xBF)
	{
		delayMs(5);
		while(P1 == 0xBF);
		
		switch(P1)
		{
			case 0xB7:num = 5;break;
			case 0xBB:num = 6;break;
			case 0xBD:num = 7;break;
			case 0xBE:num = 8;break;
		}
		delayMs(5);
		while(P1 != 0xBF);
	}
	
	P1 = 0xDF;  //μúèyDD
	if(P1 != 0xDF)
	{
		delayMs(5);
		while(P1 == 0xDF);
		
		switch(P1)
		{
			case 0xD7:num = 9;break;
			case 0xDB:num = 10;break;
			case 0xDD:num = 11;break;
			case 0xDE:num = 12;break;
		}
		delayMs(5);
		while(P1 != 0xDF);
	}
	
	
	P1 = 0xEF;  //μú??DD
	if(P1 != 0xEF)
	{
		delayMs(5);
		while(P1 == 0xEF);
		
		switch(P1)
		{
			case 0xE7:num = 13;break;
			case 0xEB:num = 14;break;
			case 0xED:num = 15;break;
			case 0xEE:num = 16;break;
		}
		delayMs(5);
		while(P1 != 0xEF);
	}

	return num;
}

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}
	