#include <reg51.h>

//ͨ��ADCת�������ֵ�ѹ����LED1-LED8��Ȼ��ͨ��DAC�����ֵ�ѹת����ģ���ѹ��ʾ��������ϣ�������DA1���LED����

//xpt2046
sbit XPT_DIN = P3^4;
sbit XPT_CS = P3^5;
sbit XPT_DCLK = P3^6;
sbit XPT_DOUT = P3^7;


//����ܿ���λ
sbit LSBA = P2^2;
sbit LSBB = P2^3;
sbit LSBC = P2^4;


char code table[] = {0x3f, 0x06, 0x5b, 0x4f,
										0x66, 0x6d, 0x7d, 0x07,
										0x7f, 0x6f, 0x10};

void delayMs(unsigned int);
int xpt2046_read_adc_value(int cmd);
int adcConvertVoltage(int adcVaule);
void displayVoltage(int volt);

int main()
{
	int adcValue = 0;
	float volt = 0.0f;
	
	while(1)
	{
		//0x9F  8λ�ֱ���    0x97  12λ�ֱ���
		adcValue = xpt2046_read_adc_value(0x9F);  //8λ
		//adcValue = xpt2046_read_adc_value(0x97);  	//12λ	
		P2 = adcValue;   //��ADת���������źŸ�P2
		volt = adcConvertVoltage(adcValue);  //�������ź�ת����ģ���ź�
		displayVoltage(volt);
	}
	return 0;
}


//��ʼ��xpt2046
void XPTInit()
{
	XPT_CS = 0;
	XPT_DIN = 0;
	XPT_DCLK = 0;
	XPT_DOUT = 0;
}

//xpt2046д���ݲ���
void xpt2046_write_data(int cmd)
{
	int i = 0;
	XPT_CS = 0;
	XPT_DCLK = 0;
	XPT_DIN = 0;
	delayMs(5);
	
	for(i=7;i>=0;i--)
	{
		XPT_DCLK = 0;
		XPT_DIN = (cmd >> i) & 1;
		XPT_DCLK = 1;
	}
	
	XPT_DIN = 0;
}

//xpt2046�����ݲ���������8λ��12λ
int xpt2046_read_data()
{
	int readData = 0;
	int i = 0;
	XPT_DOUT = 0;

	//8λ
	for(i=7;i>=0;i--)
	{
		XPT_DCLK = 0;
		readData = XPT_DOUT | (readData << 1);
		XPT_DCLK = 1;
	}
	
	//12λ
//	for(i=11;i>=0;i--)
//	{
//		XPT_DCLK = 0;
//		readData = XPT_DOUT | (readData << 1);
//		XPT_DCLK = 1;
//	}

	XPT_DOUT = 1;
	
	return readData;
}


//xpt2046дָ�ֱ�ӷ��������ź�
int xpt2046_read_adc_value(int cmd)
{
	int adcValue;
	XPTInit();
	xpt2046_write_data(cmd);
	delayMs(10);
	XPT_DCLK = 0;
	XPT_DCLK = 1;
	
	adcValue = xpt2046_read_data();
	
	XPT_CS = 1;
	XPT_DCLK = 0;
	
	return adcValue;
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

int adcConvertVoltage(int adcVaule)
{
	float adc_volt = 5.0 * adcVaule / 256; //8λ
	//float adc_volt = 5.0 * adcVaule / 4096; //12λ
	int adc_magnify = adc_volt * 100;
	return adc_magnify;
}

void bitSelection(int num)
{
	switch(num)
	{
		case 1:
			LSBA = 0;
			LSBB = 0;
			LSBC = 0;
			//P2 = 0x00;
			break;
		case 2:
			LSBA = 1;
			LSBB = 0;
			LSBC = 0;
			//P2 = 0x04;
			break;
		case 3:
			LSBA = 0;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x08;
			break;
		case 4:
			LSBA = 1;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x0c;
			break;
		case 5:
			LSBA = 0;
			LSBB = 0;
			LSBC = 1;
			//P2 = 0x10;
			break;
		case 6:
			LSBA = 0;
			LSBB = 1;
			LSBC = 0;
			//P2 = 0x14;
			break;
		case 7:
			LSBA = 0;
			LSBB = 1;
			LSBC = 1;
			//P2 = 0x18;
			break;
		case 8:
			LSBA = 1;
			LSBB = 1;
			LSBC = 1;
			//P2 = 0x1c;
			break;
	}
}

void displayVoltage(int volt)
{
	int adcInteger = volt / 100;
	int adcDecimal = volt % 100;
	int i = 0;
	char dat[5] = {0};
	dat[0] = 0x3e;  //��ʾ��λ V
	dat[1] = table[adcDecimal % 10];  //С�����ֵĵڶ�λ
	dat[2] = table[adcDecimal / 10];  //С�����ֵĵ�һλ
	dat[3] = table[adcInteger % 10] | 0x80;  //�������ֵĵڶ�λ����С����
	dat[4] = table[adcInteger / 10];   //�������ֵĵ�һλ
	
	for(i = 0;i < 5;i++)
	{
		bitSelection(i+1);   //ѡ����Ӧ���������ʾ
		P0 = dat[i];   //����ܶ�ѡ��ֵ
		delayMs(1);    //��ʱ1ms���ȴ���ʾ�ȶ�
		P0 = 0x00;     //��Ӱ
	}
}