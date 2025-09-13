#include <reg51.h>
#include <stdio.h>

//����UART��ʽʵ��printf��scanf����

void delayMs(unsigned int ms)
{
	int i = 0;
	int j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}

int main()
{
	char receiveData = 0;
	//���ö�ʱ/������T1
	EA = 1;  //�����ж�
	ES = 1;  //�򿪴����ж�����λ
	
	//���ô���UART	
	SCON = 0x50;     //UART�Ĺ�����ʽ����ʽ1(10λ�첽�շ���)
	PCON = 0x00;   //�����ʱ���λΪ0
	
	TH1 = 0xFD;  //������9600
	TL1 = 0xFD;
	
	//ET1 = 1; //���ж��������λ
	TMOD = 0x20;   //���ö�ʱ/�������Ĺ�����ʽ��8λ�Զ���װ

	TR1 = 1;  //��ʱ/��������ʼ����
	
	while(1)
	{
		scanf("%c",&receiveData);
		printf("------>\n");
		printf("---%c---\n", receiveData);
		//delayMs(1000);
	}
	
	return 0;
}


void UART_SendData(unsigned char dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}

char UART_ReadData()
{
	char dat;
	while(!RI);
	RI = 0;
	dat = SBUF;
	
	return dat;
}

char putchar(char ch)
{
	UART_SendData(ch);
	return ch;
}

char _getkey(void)
{
	return UART_ReadData();
}
